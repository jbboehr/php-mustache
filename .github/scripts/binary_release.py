"""Prepare pinned libmustache SDKs and publish tested PIE binary packages."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import tarfile
import tempfile
import urllib.parse
import urllib.request
import zipfile


ROOT = Path(__file__).resolve().parents[2]
WINDOWS_PHP = ("8.0", "8.1", "8.2", "8.3", "8.4", "8.5")
MACOS_PHP = ("8.2", "8.3", "8.4", "8.5")
LICENSE_NAMES = ("php-mustache", "libmustache", "cista", "nlohmann-json", "xxhash")


def load_sdk_manifest(root=ROOT):
    manifest = json.loads((root / ".github/libmustache-sdk.json").read_text())
    lock = json.loads((root / "flake.lock").read_text())
    node = lock["nodes"][lock["root"]]["inputs"]["libmustache"]
    locked = lock["nodes"][node]["locked"]
    if (locked.get("owner"), locked.get("repo"), locked.get("rev")) != (
        "jbboehr", "libmustache", manifest["revision"]
    ):
        raise ValueError("The binary SDK revision must match libmustache in flake.lock")
    return manifest


def windows_toolset(php_version):
    return "v142" if php_version in WINDOWS_PHP[:4] else "v143"


def metadata(environment, root=ROOT):
    load_sdk_manifest(root)
    header = (root / "php_mustache.h").read_text()
    version = re.search(r'^#define PHP_MUSTACHE_VERSION "([0-9]+\.[0-9]+\.[0-9]+)"$', header, re.M)
    if version is None:
        raise ValueError("Cannot read the extension release version")
    tag = "v" + version[1]
    ref = environment.get("GITHUB_REF", "")
    if ref.startswith("refs/tags/") and ref != "refs/tags/" + tag:
        raise ValueError("Tag does not match PHP_MUSTACHE_VERSION")
    release_branch = ref == "refs/heads/release" or ref.startswith("refs/heads/release/")
    if ref.startswith("refs/heads/release/") and ref.removeprefix("refs/heads/release/") not in (tag, version[1]):
        raise ValueError("Release branch does not match PHP_MUSTACHE_VERSION")
    package_ref = tag if release_branch or ref.startswith("refs/tags/") else environment.get("GITHUB_SHA", tag)
    return {
        "package-ref": package_ref,
        "windows": {"include": [
            {"php": php, "ts": ts, "sdk": f"windows-x64-{windows_toolset(php)}-md-static"}
            for php in WINDOWS_PHP for ts in ("nts", "ts")
        ]},
        "macos": {"php": list(MACOS_PHP), "ts": ["nts", "zts"]},
    }


def prepare_sdk(platform, cache, destination, manifest):
    suffix = ".zip" if platform.startswith("windows-") else ".tar.gz"
    name = f"libmustache-{manifest['version']}-{platform}{suffix}"
    expected = manifest["sha256"][platform]
    cache.mkdir(parents=True, exist_ok=True)
    archive_path = cache / name
    if archive_path.exists() and hashlib.sha256(archive_path.read_bytes()).hexdigest() != expected:
        print(f"Discarding corrupt cached SDK: {name}")
        archive_path.unlink()
    if not archive_path.exists():
        url = f"https://github.com/jbboehr/libmustache/releases/download/v{manifest['version']}/{name}"
        with urllib.request.urlopen(url, timeout=60) as response:
            data = response.read()
        if hashlib.sha256(data).hexdigest() != expected:
            raise ValueError(f"SDK checksum mismatch: {name}")
        with tempfile.TemporaryDirectory(dir=cache) as temporary:
            staged = Path(temporary) / name
            staged.write_bytes(data)
            staged.replace(archive_path)
    if hashlib.sha256(archive_path.read_bytes()).hexdigest() != expected:
        raise ValueError(f"SDK checksum mismatch: {name}")
    destination.mkdir(parents=True, exist_ok=False)
    if suffix == ".zip":
        with zipfile.ZipFile(archive_path) as archive:
            archive.extractall(destination)
    else:
        with tarfile.open(archive_path) as archive:
            archive.extractall(destination, filter="data")
    library = "lib/mustache_static.lib" if suffix == ".zip" else "lib/libmustache.a"
    for path in (library, "include/mustache/mustache_config.h", "include/mustache/mustache_export.hpp",
                 "include/mustache/mustache.hpp", "LICENSE.md",
                 "licenses/cista/LICENSE", "licenses/nlohmann-json/LICENSE", "licenses/xxhash/LICENSE"):
        if not (destination / path).is_file():
            raise ValueError(f"Missing SDK file: {path}")


def license_notices(sdk, root=ROOT):
    sources = [("php-mustache", root / "LICENSE.md"), ("libmustache", sdk / "LICENSE.md")]
    sources.extend((name, sdk / "licenses" / name / "LICENSE") for name in LICENSE_NAMES[2:])
    return "\n\n".join(f"=== {name} ===\n{path.read_text(encoding='utf-8')}" for name, path in sources) + "\n"


def expected_packages(package_ref):
    if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._-]*", package_ref):
        raise ValueError("Invalid package reference")
    packages = {}
    for php in WINDOWS_PHP:
        compiler = "vs16" if windows_toolset(php) == "v142" else "vs17"
        for ts in ("nts", "ts"):
            stem = f"php_mustache-{package_ref}-{php}-{ts}-{compiler}-x86_64"
            packages[stem + ".zip"] = stem + ".dll"
    for php in MACOS_PHP:
        for ts in ("nts", "zts"):
            packages[f"php_mustache-{package_ref}_php{php}-arm64-darwin-bsdlibc-{ts}.zip"] = "mustache.so"
    return packages


def validate_packages(directory, package_ref):
    expected = expected_packages(package_ref)
    if {path.name for path in directory.glob("*.zip")} != set(expected):
        raise ValueError("Release packages do not contain the complete build matrix")
    assets = []
    for name, binary in sorted(expected.items()):
        path = directory / name
        with zipfile.ZipFile(path) as archive:
            if archive.testzip() is not None or binary not in archive.namelist() or "LICENSE" not in archive.namelist():
                raise ValueError(f"Invalid package contents: {name}")
            if archive.getinfo(binary).file_size == 0:
                raise ValueError(f"Empty extension in package contents: {name}")
            licenses = archive.read("LICENSE").decode("utf-8-sig")
            sections = re.split(r"(?m)^=== ([^\r\n]+) ===\r?$", licenses)
            notices = dict(zip(sections[1::2], sections[2::2]))
            if not all(notices.get(notice, "").strip() for notice in LICENSE_NAMES):
                raise ValueError(f"Missing dependency license notices: {name}")
        digest = hashlib.sha256(path.read_bytes()).hexdigest()
        checksum = path.with_name(name + ".sha256")
        checksum.write_text(f"{digest} *{name}\n", encoding="ascii")
        assets.extend([str(path), str(checksum)])
    return assets


def gh(*args):
    return subprocess.run(["gh", *args], check=True, capture_output=True, text=True).stdout


def check_source_ref(repo, ref, sha):
    source = json.loads(gh("api", f"repos/{repo}/commits/{urllib.parse.quote(ref, safe='')}"))
    if source["sha"] != sha:
        raise ValueError("The source ref no longer points to the tested commit")


def publish(directory, package_ref, mode, environment):
    event = environment.get("GITHUB_EVENT_NAME")
    ref = environment.get("GITHUB_REF", "")
    draft = mode == "draft"
    release_branch = ref == "refs/heads/release" or ref.startswith("refs/heads/release/")
    if not re.fullmatch(r"v[0-9]+\.[0-9]+\.[0-9]+", package_ref):
        raise ValueError("Publication requires a versioned package reference")
    if draft:
        if event != "push" or not release_branch:
            raise ValueError("Drafts require a release branch push")
    elif mode != "publish" or event != "push" or ref != "refs/tags/" + package_ref:
        raise ValueError("Releases are published only for a matching version tag push")
    assets = validate_packages(directory, package_ref)
    repo = environment["GITHUB_REPOSITORY"]
    sha = environment["GITHUB_SHA"]
    check_source_ref(repo, ref, sha)
    if draft:
        tags = json.loads(gh("api", f"repos/{repo}/git/matching-refs/tags/{package_ref}"))
        if any(tag["ref"] == "refs/tags/" + package_ref for tag in tags):
            raise ValueError("Version is already tagged; use the tag's CI run")
    try:
        existing = json.loads(gh("release", "view", package_ref, "--repo", repo, "--json", "isDraft"))
    except subprocess.CalledProcessError as error:
        if "release not found" not in (error.stderr or "").lower():
            raise
        existing = None
    if existing is not None and not existing["isDraft"]:
        raise ValueError("Release is already published; refusing to replace its assets")
    if existing is None:
        gh("release", "create", package_ref, "--repo", repo, "--draft", "--target", sha,
           "--title", package_ref, "--generate-notes")
    gh("release", "upload", package_ref, "--repo", repo, "--clobber", *assets)
    # Remove obsolete binary assets only after the replacement matrix is uploaded.
    current = json.loads(gh("release", "view", package_ref, "--repo", repo, "--json", "isDraft,assets"))
    if not current["isDraft"]:
        raise ValueError("Release is already published; refusing to change it after upload")
    expected = {Path(asset).name for asset in assets}
    for asset in current.get("assets", []):
        if asset["name"].startswith("php_mustache-") and asset["name"] not in expected:
            gh("release", "delete-asset", package_ref, asset["name"], "--repo", repo, "--yes")
    check_source_ref(repo, ref, sha)
    options = ("--target", sha) if draft else ("--draft=false", "--verify-tag")
    gh("release", "edit", package_ref, "--repo", repo, *options)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    commands.add_parser("metadata")
    sdk = commands.add_parser("sdk")
    sdk.add_argument("platform")
    sdk.add_argument("cache", type=Path)
    sdk.add_argument("destination", type=Path)
    for command in ("validate", "draft", "publish"):
        subparser = commands.add_parser(command)
        subparser.add_argument("directory", type=Path)
        subparser.add_argument("package_ref")
    args = parser.parse_args()
    if args.command == "metadata":
        outputs = metadata(os.environ)
        with open(os.environ.get("GITHUB_OUTPUT", os.devnull), "a", encoding="utf-8") as output:
            for key, value in outputs.items():
                value = json.dumps(value, separators=(",", ":")) if isinstance(value, dict) else value
                print(f"{key}={value}")
                output.write(f"{key}={value}\n")
    elif args.command == "sdk":
        prepare_sdk(args.platform, args.cache, args.destination, load_sdk_manifest())
        (ROOT / "LICENSE").write_text(license_notices(args.destination), encoding="utf-8")
    elif args.command == "validate":
        assets = validate_packages(args.directory, args.package_ref)
        print(f"Validated {len(assets) // 2} PIE packages and wrote SHA-256 checksums")
    else:
        publish(args.directory, args.package_ref, args.command, os.environ)


if __name__ == "__main__":
    main()
