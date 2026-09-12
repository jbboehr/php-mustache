# Development notes

These notes hold current implementation decisions and unfinished work.
The [PHP API guide](../php-api.md) documents the caller-facing contracts.

- [libmustache compatibility](libmustache-compatibility.md) covers matched builds,
  AST source ownership, callback results, and cache migration.
- [PIE binary releases](binary-releases.md) covers pinned SDKs, platform packages,
  release branch drafts, and tag publication.
- [Data conversion ownership](data-conversion-ownership.md) records capture,
  constructor publication, and destructor-method rules.
- [Partial-map budgets](partial-map-budget.md) records current accounting and
  the reasons default thresholds remain deferred.
- [Zend bailout ownership](zend-bailout-ownership.md) and the
  [adapter contract](zend-bailout-adapter-contract.md) retain the unresolved F7
  design and integration gate. The native boundary prototype remains with them.
- [Issue 68 resolution](issue-68-resolution.md) is the prepared, unposted comment.
- [Upgrading to 0.10.0](../upgrading.md) covers application and cache migration.
- [Benchmarks](../../benchmarks/README.md) and [fuzzing](../../fuzz/README.md)
  describe their existing tools.

## Remaining planned work

The release target is **0.10.1**, with template evaluation retained as the
callback-string default. The version fields, package notes, changelog, and
migration guide are prepared. The date in the package and extension metadata
is provisional until publication.

Before tagging:

1. Keep the libmustache source pin and binary SDK manifest aligned with
   `v0.6.2` (`c155e10`). Review the
   [deployment and cache constraints](libmustache-compatibility.md) and retest
   any dependency change.
2. Complete the platform checks for the release candidate. Windows, macOS,
   ARM64, and Docker container runs remain unverified locally.
3. Set the actual publication date in `package.xml`, `php_mustache.h`, and
   `CHANGELOG.md`. Keep version 0.10.1 consistent with `nix/derivation.nix`.
   Rebuild the PECL package and verify the final PECL and PIE artifacts after
   any source or metadata change.

Pushing `v0.10.1` now requests automatic publication after CI succeeds, as
described in [binary releases](binary-releases.md). Composer
derives the PIE package version from the tag, so `composer.json` has no hardcoded
version. Once the release is available, update the issue 68 draft's unreleased
wording, publish the comment, and close the issue.

Zend bailout integration and arena work remain deferred. Default partial-map
thresholds and complete memory/node accounting also remain deferred, with
reopening criteria in their design notes.

## Verification

Run the repository checks from the project root:

```sh
nix flake check --keep-going --no-write-lock-file
nix develop --no-write-lock-file --command pre-commit run --all-files
```

Run runtime checks explicitly, as CI does. For example, on x86_64 Linux:

```sh
nix build --no-link --keep-going --no-write-lock-file \
  .#checks.x86_64-linux.php83-gcc \
  .#checks.x86_64-linux.php83-gcc-sanitized \
  .#php83-archive-benchmark .#php83-gcc-valgrind
```

The complete compiler, PHP-version, coverage, and sanitizer matrix is listed by
`nix eval --json .#githubActions.matrix`. Build every applicable matrix attribute
for release verification. A successful `nix flake check` alone does not establish
that the runtime suites executed.

Available results depend on the host platform. Git-backed flake inputs and
`pre-commit --all-files` omit untracked files, so include new files explicitly
when verifying a working tree.

## Historical reports

Completed project reviews, dependency-update reports, and implementation plans
are retained in Git history:

```sh
git log -- docs/development/
git show <commit>:docs/development/<file>
```

Keep new notes focused on durable decisions or unfinished work. Test counts,
temporary log paths, and completed step-by-step plans belong in review or commit
records once the work is done.
