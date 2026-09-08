# Development notes

These notes hold current implementation decisions and unfinished work.
The [PHP API guide](../php-api.md) documents the caller-facing contracts.

- [libmustache compatibility](libmustache-compatibility.md) covers matched builds,
  AST source ownership, callback results, and cache migration.
- [Data conversion ownership](data-conversion-ownership.md) records capture,
  constructor publication, and destructor-method rules.
- [Partial-map budgets](partial-map-budget.md) records current accounting and
  the reasons default thresholds remain deferred.
- [Zend bailout ownership](zend-bailout-ownership.md) and the
  [adapter contract](zend-bailout-adapter-contract.md) retain the unresolved F7
  design and integration gate. The native boundary prototype remains with them.
- [Issue 68 resolution](issue-68-resolution.md) is the prepared, unposted comment.
- [Benchmarks](../../benchmarks/README.md) and [fuzzing](../../fuzz/README.md)
  describe their existing tools.

## Remaining planned work

Before tagging a release:

1. Select a compatible libmustache release or exact pin and rebuild the extension
   against it. Review the [deployment and cache constraints](libmustache-compatibility.md).
2. Update the extension version, package metadata, changelog, and migration
   guidance. The implementation keeps template evaluation as the callback-string
   default. The earlier proposal to make literal mode the release default remains
   a separate compatibility decision.
3. Verify the packaged PECL and PIE installation paths for the release artifacts.

The lambda string-mode API, explicit result classes, and compatibility
implementation are complete. Publishing the issue 68 comment and closing the
issue remain separate external actions.

Zend bailout integration and arena work remain deferred. Default partial-map
thresholds and complete memory/node accounting also remain deferred, with
reopening criteria in their design notes.

## Verification

Run the repository checks from the project root:

```sh
nix flake check --keep-going --no-write-lock-file
nix develop --no-write-lock-file --command pre-commit run --all-files
```

The flake defines the compiler, PHP-version, coverage, and sanitizer checks.
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
