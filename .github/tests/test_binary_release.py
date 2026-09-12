"""Check SDK integrity, PIE package contents, and release publication gates."""

import hashlib
import importlib.util
import io
import json
from pathlib import Path
import re
import subprocess
import sys
import tarfile
import tempfile
import unittest
from unittest.mock import patch
import zipfile


ROOT = Path(__file__).resolve().parents[2]
sys.dont_write_bytecode = True
spec = importlib.util.spec_from_file_location("binary_release", ROOT / ".github/scripts/binary_release.py")
release = importlib.util.module_from_spec(spec)
spec.loader.exec_module(release)


class ReleaseFixture(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory(prefix="mustache-release-test-")
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)

    def packages(self, ref="v0.10.1"):
        notices = "\n\n".join(
            f"=== {name} ===\nFixture license terms for {name}."
            for name in release.LICENSE_NAMES
        )
        for name, binary in release.expected_packages(ref).items():
            with zipfile.ZipFile(self.root / name, "w") as archive:
                archive.writestr(binary, b"extension fixture")
                archive.writestr("LICENSE", notices)
        return self.root


class BinaryReleaseTest(ReleaseFixture):
    def metadata_root(self):
        root = self.root / "project"
        (root / ".github").mkdir(parents=True)
        (root / "php_mustache.h").write_text('#define PHP_MUSTACHE_VERSION "0.10.1"\n')
        for path in ("flake.lock", ".github/libmustache-sdk.json"):
            (root / path).write_bytes((ROOT / path).read_bytes())
        return root

    def test_metadata_uses_version_for_release_refs_and_sha_for_other_runs(self):
        root = self.metadata_root()
        for ref, expected in (("refs/heads/release/v0.10.1", "v0.10.1"),
                              ("refs/heads/release/0.10.1", "v0.10.1"),
                              ("refs/tags/v0.10.1", "v0.10.1"),
                              ("refs/pull/1/merge", "a" * 40)):
            with self.subTest(ref=ref):
                self.assertEqual(release.metadata({"GITHUB_REF": ref, "GITHUB_SHA": "a" * 40}, root)["package-ref"], expected)

    def test_metadata_rejects_version_mismatch(self):
        root = self.metadata_root()
        for ref in ("refs/tags/v0.10.0", "refs/heads/release/v0.10.0"):
            with self.subTest(ref=ref), self.assertRaisesRegex(ValueError, "VERSION"):
                release.metadata({"GITHUB_REF": ref}, root)

    def test_package_names_cover_supported_php_and_thread_modes(self):
        names = release.expected_packages("v0.10.1")
        self.assertEqual(len(names), 20)
        self.assertIn("php_mustache-v0.10.1-8.0-nts-vs16-x86_64.zip", names)
        self.assertIn("php_mustache-v0.10.1-8.3-ts-vs16-x86_64.zip", names)
        self.assertIn("php_mustache-v0.10.1-8.5-ts-vs17-x86_64.zip", names)
        self.assertEqual(names["php_mustache-v0.10.1_php8.2-arm64-darwin-bsdlibc-zts.zip"], "mustache.so")

    def test_complete_packages_validate_and_write_checksums(self):
        assets = release.validate_packages(self.packages(), "v0.10.1")
        self.assertEqual(len(assets), 40)
        archive = self.root / "php_mustache-v0.10.1-8.0-nts-vs16-x86_64.zip"
        digest = hashlib.sha256(archive.read_bytes()).hexdigest()
        self.assertEqual(Path(str(archive) + ".sha256").read_text(), f"{digest} *{archive.name}\n")

    def test_missing_or_wrong_version_package_is_rejected(self):
        self.packages()
        next(self.root.glob("*.zip")).rename(self.root / "php_mustache-wrong-version.zip")
        with self.assertRaisesRegex(ValueError, "matrix"):
            release.validate_packages(self.root, "v0.10.1")

    def test_archive_must_contain_extension_and_dependency_licenses(self):
        self.packages()
        name = "php_mustache-v0.10.1-8.0-nts-vs16-x86_64.zip"
        for contents in ({"LICENSE": "licenses"}, {name[:-4] + ".dll": "extension"}):
            with self.subTest(contents=contents):
                with zipfile.ZipFile(self.root / name, "w") as archive:
                    for path, data in contents.items():
                        archive.writestr(path, data)
                with self.assertRaisesRegex(ValueError, "contents|license"):
                    release.validate_packages(self.root, "v0.10.1")

    def test_dependency_names_without_license_text_are_rejected(self):
        self.packages()
        name = "php_mustache-v0.10.1-8.0-nts-vs16-x86_64.zip"
        binary = name[:-4] + ".dll"
        headings_only = "\n".join(f"=== {notice} ===" for notice in release.LICENSE_NAMES)
        with zipfile.ZipFile(self.root / name, "w") as archive:
            archive.writestr(binary, b"extension fixture")
            archive.writestr("LICENSE", headings_only)
        with self.assertRaisesRegex(ValueError, "license"):
            release.validate_packages(self.root, "v0.10.1")

    def test_each_dependency_requires_its_heading_and_nonempty_notice(self):
        self.packages()
        name, binary = next(iter(release.expected_packages("v0.10.1").items()))
        for dependency in release.LICENSE_NAMES:
            for missing_heading in (False, True):
                with self.subTest(dependency=dependency, missing_heading=missing_heading):
                    notices = []
                    for notice in release.LICENSE_NAMES:
                        heading = f"=== {notice} ===\n"
                        body = f"Fixture license terms for {notice}."
                        if notice == dependency:
                            if missing_heading:
                                heading = ""
                            else:
                                body = " \t\r\n"
                        notices.append(heading + body)
                    with zipfile.ZipFile(self.root / name, "w") as archive:
                        archive.writestr(binary, b"extension fixture")
                        archive.writestr("LICENSE", "\n\n".join(notices))
                    with self.assertRaisesRegex(ValueError, "license"):
                        release.validate_packages(self.root, "v0.10.1")

    def test_sdk_pin_must_match_flake_lock(self):
        manifest = release.load_sdk_manifest(ROOT)
        (self.root / ".github").mkdir()
        (self.root / ".github/libmustache-sdk.json").write_text(json.dumps(manifest))
        lock = json.loads((ROOT / "flake.lock").read_text())
        lock["nodes"]["libmustache"]["locked"]["rev"] = "0" * 40
        (self.root / "flake.lock").write_text(json.dumps(lock))
        with self.assertRaisesRegex(ValueError, "flake.lock"):
            release.load_sdk_manifest(self.root)

    def test_cached_sdk_and_failed_replacement_are_not_extracted(self):
        manifest = release.load_sdk_manifest(ROOT)
        name = f"libmustache-{manifest['version']}-windows-x64-v142-md-static.zip"
        (self.root / name).write_bytes(b"corrupt cache")
        destination = self.root / "sdk"
        with patch.object(release.urllib.request, "urlopen", return_value=io.BytesIO(b"bad replacement")) as download:
            with self.assertRaisesRegex(ValueError, "checksum"):
                release.prepare_sdk("windows-x64-v142-md-static", self.root, destination, manifest)
            download.assert_called_once()
        self.assertFalse(destination.exists())
        self.assertFalse((self.root / name).exists())

    def sdk_fixture(self, platform, missing=None):
        files = {
            "include/mustache/mustache_config.h": "config",
            "include/mustache/mustache_export.hpp": "exports",
            "include/mustache/mustache.hpp": "header",
            "LICENSE.md": "upstream license",
            "licenses/cista/LICENSE": "cista license",
            "licenses/nlohmann-json/LICENSE": "json license",
            "licenses/xxhash/LICENSE": "xxhash license",
        }
        data = io.BytesIO()
        if platform.startswith("windows-"):
            files["lib/mustache_static.lib"] = "library"
            files.pop(missing, None)
            with zipfile.ZipFile(data, "w") as archive:
                for name, content in files.items():
                    archive.writestr(name, content)
        else:
            files["lib/libmustache.a"] = "library"
            files.pop(missing, None)
            with tarfile.open(fileobj=data, mode="w:gz") as archive:
                for name, content in files.items():
                    member = tarfile.TarInfo("./" + name)
                    payload = content.encode()
                    member.size = len(payload)
                    archive.addfile(member, io.BytesIO(payload))
        payload = data.getvalue()
        manifest = {"version": "0.6.2", "sha256": {platform: hashlib.sha256(payload).hexdigest()}}
        return payload, manifest

    def test_sdk_download_and_cached_reuse(self):
        for platform, library in (("windows-x64-v142-md-static", "lib/mustache_static.lib"),
                                  ("macos-aarch64-static", "lib/libmustache.a")):
            with self.subTest(platform=platform):
                payload, manifest = self.sdk_fixture(platform)
                sdk = self.root / platform
                with patch.object(release.urllib.request, "urlopen", return_value=io.BytesIO(payload)) as download:
                    release.prepare_sdk(platform, self.root, sdk, manifest)
                    download.assert_called_once()
                with patch.object(release.urllib.request, "urlopen") as download:
                    release.prepare_sdk(platform, self.root, self.root / (platform + "-cached"), manifest)
                    download.assert_not_called()
                self.assertEqual((sdk / library).read_text(), "library")
                licenses = release.license_notices(sdk, ROOT)
                for text in ("upstream license", "cista license", "json license", "xxhash license"):
                    self.assertIn(text, licenses)

    def test_hash_valid_sdk_missing_required_library_is_rejected(self):
        for platform, library in (("windows-x64-v142-md-static", "lib/mustache_static.lib"),
                                  ("macos-aarch64-static", "lib/libmustache.a")):
            with self.subTest(platform=platform):
                payload, manifest = self.sdk_fixture(platform, missing=library)
                with patch.object(release.urllib.request, "urlopen", return_value=io.BytesIO(payload)):
                    with self.assertRaisesRegex(ValueError, re.escape(f"Missing SDK file: {library}")):
                        release.prepare_sdk(platform, self.root, self.root / platform, manifest)

    def test_corrupt_cached_sdk_is_replaced_by_one_verified_download(self):
        platform = "windows-x64-v142-md-static"
        payload, manifest = self.sdk_fixture(platform)
        cached = self.root / f"libmustache-{manifest['version']}-{platform}.zip"
        cached.write_bytes(b"corrupt cache")
        with patch.object(release.urllib.request, "urlopen", return_value=io.BytesIO(payload)) as download:
            release.prepare_sdk(platform, self.root, self.root / "sdk", manifest)
            download.assert_called_once()
        self.assertEqual(cached.read_bytes(), payload)
        self.assertEqual((self.root / "sdk/lib/mustache_static.lib").read_text(), "library")

    def test_failed_cache_write_does_not_leave_a_partial_archive(self):
        platform = "windows-x64-v142-md-static"
        payload, manifest = self.sdk_fixture(platform)
        cached = self.root / f"libmustache-{manifest['version']}-{platform}.zip"
        write_bytes = Path.write_bytes

        def interrupted_write(path, data):
            write_bytes(path, data[:10])
            raise OSError("interrupted write")

        with patch.object(release.urllib.request, "urlopen", return_value=io.BytesIO(payload)):
            with patch.object(Path, "write_bytes", interrupted_write):
                with self.assertRaisesRegex(OSError, "interrupted write"):
                    release.prepare_sdk(platform, self.root, self.root / "sdk", manifest)
        self.assertFalse(cached.exists())
        self.assertFalse((self.root / "sdk").exists())

    def test_interrupted_download_does_not_create_cache_or_destination(self):
        platform = "windows-x64-v142-md-static"
        payload, manifest = self.sdk_fixture(platform)
        cached = self.root / f"libmustache-{manifest['version']}-{platform}.zip"

        class InterruptedResponse(io.BytesIO):
            def read(self, *args, **kwargs):
                raise OSError("connection lost")

        with patch.object(release.urllib.request, "urlopen",
                          return_value=InterruptedResponse(payload)) as download:
            with self.assertRaisesRegex(OSError, "connection lost"):
                release.prepare_sdk(platform, self.root, self.root / "sdk", manifest)
            download.assert_called_once()
        self.assertFalse(cached.exists())
        self.assertFalse((self.root / "sdk").exists())

    def test_failed_atomic_cache_replacement_leaves_no_canonical_archive(self):
        platform = "windows-x64-v142-md-static"
        payload, manifest = self.sdk_fixture(platform)
        cached = self.root / f"libmustache-{manifest['version']}-{platform}.zip"
        staged_paths = []

        def interrupted_replace(staged, target):
            staged_paths.append(staged)
            self.assertEqual(target, cached)
            raise OSError("atomic replacement failed")

        with patch.object(release.urllib.request, "urlopen", return_value=io.BytesIO(payload)):
            with patch.object(Path, "replace", autospec=True, side_effect=interrupted_replace):
                with self.assertRaisesRegex(OSError, "atomic replacement failed"):
                    release.prepare_sdk(platform, self.root, self.root / "sdk", manifest)
        self.assertEqual(len(staged_paths), 1)
        self.assertTrue(staged_paths[0].is_relative_to(self.root))
        self.assertFalse(cached.exists())
        self.assertFalse((self.root / "sdk").exists())


class WorkflowContractTest(unittest.TestCase):
    def test_release_waits_for_coverage_reporting(self):
        workflow = (ROOT / ".github/workflows/ci.yml").read_text()
        release_job = re.search(r"(?ms)^    release:\n(.*?)(?=^    [A-Za-z0-9_-]+:\n|\Z)", workflow)
        self.assertIsNotNone(release_job)
        self.assertRegex(release_job[1], r"(?s)^.*?needs:.*?coveralls-finished")


class PublicationTest(ReleaseFixture):
    def setUp(self):
        super().setUp()
        self.packages()
        self.environment = {
            "GITHUB_EVENT_NAME": "push", "GITHUB_REF_TYPE": "branch",
            "GITHUB_REF": "refs/heads/release/v0.10.1", "GITHUB_REF_NAME": "release/v0.10.1",
            "GITHUB_SHA": "1" * 40, "GITHUB_REPOSITORY": "example/php-mustache",
        }
        self.commands = []
        self.existing = {"isDraft": True}
        self.source_sha = self.environment["GITHUB_SHA"]
        self.version_tag_exists = False
        self.upload_fails = False
        self.move_source_after_upload = False
        self.publish_after_upload = False

    def gh(self, *args):
        self.commands.append(args)
        if args[:2] == ("release", "view"):
            return json.dumps(self.existing)
        if args[0] == "api":
            if "/commits/" in args[1]:
                return json.dumps({"sha": self.source_sha})
            tags = [{"ref": "refs/tags/v0.10.1"}] if self.version_tag_exists else []
            return json.dumps(tags)
        if args[:2] == ("release", "upload") and self.upload_fails:
            raise subprocess.CalledProcessError(1, args)
        if args[:2] == ("release", "upload") and self.move_source_after_upload:
            self.source_sha = "2" * 40
        if args[:2] == ("release", "upload") and self.publish_after_upload:
            self.existing["isDraft"] = False
        return ""

    def publish(self, mode="draft"):
        with patch.object(release, "gh", side_effect=self.gh):
            release.publish(self.root, "v0.10.1", mode, self.environment)

    def test_release_branch_updates_draft_without_publishing_or_changing_notes(self):
        self.publish()
        edits = [command for command in self.commands if command[:2] == ("release", "edit")]
        self.assertEqual(len(edits), 1)
        self.assertIn(self.environment["GITHUB_SHA"], edits[0])
        self.assertNotIn("--draft=false", edits[0])
        self.assertNotIn("--notes", edits[0])

    def test_tag_push_publishes_only_after_upload_succeeds(self):
        self.environment.update(GITHUB_REF_TYPE="tag", GITHUB_REF="refs/tags/v0.10.1", GITHUB_REF_NAME="v0.10.1")
        self.publish("publish")
        upload = next(i for i, command in enumerate(self.commands) if command[:2] == ("release", "upload"))
        published = next(i for i, command in enumerate(self.commands) if "--draft=false" in command)
        self.assertLess(upload, published)

    def test_tag_moved_after_upload_cannot_publish_tested_artifacts(self):
        self.environment.update(GITHUB_REF_TYPE="tag", GITHUB_REF="refs/tags/v0.10.1", GITHUB_REF_NAME="v0.10.1")
        self.move_source_after_upload = True
        with self.assertRaisesRegex(ValueError, "tested commit"):
            self.publish("publish")
        source_checks = [command for command in self.commands if command[0] == "api" and "/commits/" in command[1]]
        self.assertGreaterEqual(len(source_checks), 2)
        self.assertFalse(any("--draft=false" in command for command in self.commands))

    def test_branch_moved_after_upload_cannot_update_draft(self):
        self.move_source_after_upload = True
        with self.assertRaisesRegex(ValueError, "tested commit"):
            self.publish()
        self.assertFalse(any(command[:2] == ("release", "edit") for command in self.commands))

    def test_release_published_during_upload_blocks_cleanup_and_update(self):
        self.publish_after_upload = True
        self.existing["assets"] = [{"name": "php_mustache-obsolete.zip"}]
        with self.assertRaisesRegex(ValueError, "published"):
            self.publish()
        self.assertFalse(any(command[:2] == ("release", "delete-asset") for command in self.commands))
        self.assertFalse(any(command[:2] == ("release", "edit") for command in self.commands))

    def test_failed_upload_does_not_publish(self):
        self.environment.update(GITHUB_REF_TYPE="tag", GITHUB_REF="refs/tags/v0.10.1", GITHUB_REF_NAME="v0.10.1")
        self.upload_fails = True
        with self.assertRaises(subprocess.CalledProcessError):
            self.publish("publish")
        self.assertFalse(any(command[:2] == ("release", "edit") for command in self.commands))
        self.assertFalse(any(command[:2] == ("release", "delete-asset") for command in self.commands))

    def test_existing_version_tag_blocks_draft_upload(self):
        self.version_tag_exists = True
        with self.assertRaisesRegex(ValueError, "already tagged"):
            self.publish()
        self.assertFalse(any(command[:2] == ("release", "upload") for command in self.commands))

    def test_published_release_cannot_be_overwritten(self):
        self.existing = {"isDraft": False}
        with self.assertRaisesRegex(ValueError, "published"):
            self.publish()
        self.assertFalse(any(command[:2] == ("release", "upload") for command in self.commands))

    def test_stale_source_ref_cannot_upload(self):
        self.source_sha = "2" * 40
        with self.assertRaisesRegex(ValueError, "tested commit"):
            self.publish()
        self.assertFalse(any(command[:2] == ("release", "upload") for command in self.commands))

    def test_pull_requests_cannot_draft_or_publish(self):
        self.environment["GITHUB_EVENT_NAME"] = "pull_request"
        for mode in ("draft", "publish"):
            with self.subTest(mode=mode), self.assertRaises(ValueError):
                self.publish(mode)
        self.assertEqual(self.commands, [])


if __name__ == "__main__":
    unittest.main()
