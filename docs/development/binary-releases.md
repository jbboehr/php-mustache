# PIE binary releases

CI uses libmustache's published static SDKs to build PIE packages for Windows
x64 (PHP 8.0–8.5, TS/NTS) and macOS ARM64 (PHP 8.2–8.5, ZTS/NTS). macOS
packages target macOS 15 or newer. Other Unix installations retain PIE's source
build fallback. Linux, x86, ARM64 Windows, and debug PHP binaries are not part
of this release matrix.

The package naming and Windows builder setup follow
[php-yumemi's release/v0.1.1 CI](https://github.com/jbboehr/php-yumemi/blob/release/v0.1.1/.github/workflows/ci.yml).
Release branches produce drafts and version tags publish them, following
[libmustache's release workflow](https://github.com/jbboehr/libmustache/blob/master/.github/workflows/release.yml).

## Dependency updates

[.github/libmustache-sdk.json](../../.github/libmustache-sdk.json) pins the SDK
version, source revision, and SHA-256 of each archive. Its revision must match
`flake.lock`. Update both together when adopting a new libmustache release;
copy checksums from the upstream release and verify the tag's peeled commit.
CI caches the downloaded archives and verifies their pinned checksums on every
use, including cache hits. A corrupt cached archive is discarded and downloaded
once more; the replacement must pass checksum verification before it is stored
atomically. Extraction supplies the headers and static library; no upstream
source build runs in the binary package jobs.

Windows selects the v142 `/MD` SDK for PHP 8.0–8.3 and v143 `/MD` for 8.4–8.5.
The macOS SDK supplies a position-independent static library. Packages include
the extension license and libmustache's bundled dependency notices. The macOS
job rejects extension dependencies outside system library locations, then
loads and renders with the extension extracted from the final ZIP.

The older `windows.ps1` script remains available for building the locked
libmustache source locally with CMake. The Linux, Docker, macOS source, Nix,
and Unix PIE smoke jobs continue checking source builds.

## Drafting and publishing

1. Set the extension version consistently in `php_mustache.h`, `package.xml`,
   `CHANGELOG.md`, and `nix/derivation.nix`.
2. Push the prepared commit to `release/vX.Y.Z`, `release/X.Y.Z`, or `release`.
   CI builds and tests the packages and runs the existing platform checks.
   Only after all required jobs succeed does `release.yml` validate all 20 ZIPs,
   add SHA-256 sidecars, and create or refresh the `vX.Y.Z` draft.
3. Inspect the draft's assets and edit its release notes. The draft targets the
   tested commit without creating a Git tag.
4. Push `vX.Y.Z` at the approved commit. Tag CI rebuilds and tests the packages,
   uploads the complete matrix, and publishes the draft while preserving its
   title and notes. The workflow refuses to upload when it observes that the
   release is already published.

Every run validates the full 20-package matrix with read-only credentials.
Normal branch and pull-request runs retain ZIPs as Actions artifacts; only
release branch and version tag pushes enter the publishing workflow with write
permissions and access to the optional `RELEASE_TOKEN`. Package names for normal
branch and pull-request runs use the tested commit SHA. Tags and named release
branches must agree with the extension version. Publication waits for all build,
test, and coverage-reporting jobs to succeed.

Uploads for a version are serialized. The workflow checks that the source ref
still points to the tested commit before uploading and immediately before the
final draft update or publication, and checks that the release is still a draft
after uploading, before cleanup. Release branches whose version is already
tagged are rejected. These client-side checks observe state at each request;
they cannot atomically prevent an external maintainer or API actor from
publishing a mutable draft during replacement uploads, or moving a tag after
the final check. Do not manually mutate the release draft or tag while its
workflow is uploading. Immutable releases and tag protection provide server-side
enforcement against asset changes after publication and tag movement.

Before adopting this workflow for a release, complete a
Windows/macOS CI run and verify installation through PIE from the resulting
release assets. Local Python tests cannot establish native ABI compatibility.

The workflows use `GITHUB_TOKEN`. When drafting workflow changes before they
reach `master`, GitHub can additionally require Workflows: write. As in
libmustache, an optional `RELEASE_TOKEN` secret with Contents: write and
Workflows: write can supply that permission for draft creation. Otherwise,
merge the workflow changes into `master` first. Tag publication always uses
the normal token.
