# libmustache develop update: September 7, 2026 UTC

This update starts from php-mustache `babdd42` and moves the libmustache pin
from `e6b2de00d7a3ad801eb655165eed1fd9a4352b97` to
`c43ad034850bab310d754bc3bba760cc31b08ef4`, the upstream `develop` head when
refreshed. Parsed lockfile comparison limits the change to the libmustache node.

The [seven upstream commits](https://github.com/jbboehr/libmustache/compare/e6b2de00d7a3ad801eb655165eed1fd9a4352b97...c43ad034850bab310d754bc3bba760cc31b08ef4)
add explicit lambda results, configurable interpretation of callback strings,
original section text in compiled archives, improved parser diagnostics, and
build and test changes. The reported library version remains 0.6.0. The new
virtual methods and renderer layout require rebuilding consumers against this
development snapshot. The PHP extension rebuilt without native source changes.

An independent review demonstrated a loader failure when the rebuilt extension
was forced to load the preceding libmustache snapshot. Both snapshots still
report 0.6.0 and use SONAME `libmustache.so.6`, while the new extension requires
the added `Lambda::invokeResult` symbols. Development snapshots are not binary
interchangeable. Rebuild and deploy the library and extension together; a
version-only dependency on 0.6.0 does not distinguish these revisions.

This remains a pre-release deployment limitation. Nix selects the exact library
store path, and the repository's CI and PIE smoke scripts build both components
using the locked revision. No supported repository build path was found that
mixes snapshots without an override. The upstream migration guide explicitly
requires consumer rebuilds for this unreleased ABI change. This dependency
update does not assign a new upstream SONAME or claim cross-snapshot ABI stability.

## Issue 68 support and PHP scope

Upstream now provides `LambdaResult::literal()`, `templateSource()`, and
`fromString()`. Explicit results select literal output or template evaluation;
ordinary strings inherit `LambdaStringMode`. Template evaluation remains the
default. Literal interpolation still obeys the tag's HTML escaping rules.
The [upstream migration guide](https://github.com/jbboehr/libmustache/blob/c43ad034850bab310d754bc3bba760cc31b08ef4/docs/development/abi-6-source-migration-2026-08-20.md#migrating-lambdas)
describes the C++ contract.

This update does not expose that mode or explicit result objects through PHP.
The existing binding still converts callback results to strings and overrides
the legacy string callbacks. Upstream adapts those callbacks to the new result
API, preserving existing PHP interpretation and escaping. Resolving PHP issue
68 still needs a separately reviewed PHP API and binding implementation.

## Experimental archive compatibility

Archive generation 3 retains original section callback text, including tag
spelling, comments, custom delimiters, and embedded NULs. The writer now stores
retained source buffers as well as node fields. Generation-1 and generation-2
archives are rejected; regenerate experimental caches from template source.
This changes the optional benchmark bridge's format, not the legacy public
`MustacheAST::toBinary()` format or its documented source-preservation limits.
See the [upstream archive specification](https://github.com/jbboehr/libmustache/blob/c43ad034850bab310d754bc3bba760cc31b08ef4/docs/development/cista-archive-format-v3.md).

The new [archive section-source test](../../tests/Mustache__archive-benchmark-section-source.phpt)
checks exact callback input, one invocation, and evaluated string returns for
direct roots, source partials, and source-parsed AST partials. All nine cases
reported changed input with the previous dependency and exact input with the
updated dependency. The test uses ordinary valid templates and skips builds
without the optional archive bridge.

A separate compatibility check wrote ordinary cache fixtures with the previous
library, then read them with the rebuilt extension. The old archive threw
`MustacheException` with `Unsupported libmustache archive format generation`.
An old public AST binary still rendered `Hello Ada`, and regenerating the archive
with the updated library also rendered `Hello Ada`.

An independent test review added an [archive callback compatibility test](../../tests/Mustache__archive-benchmark-lambda-compatibility.phpt).
It covers evaluated interpolation results, HTML escaping, nested data and
partial context, section helper rendering and invalidation, and PHP serialization
of archive bytes. It passes with both dependency revisions, so it is a
characterization test rather than a newly fixed behavior. Both new PHPTs passed
again against the updated archive module after review. No production defect
was demonstrated in matched library/extension builds.

## Verification

The final x86-64 Linux Nix check passed all 18 checks: pre-commit hooks and
17 runtime variants covering PHP 8.0 with GCC, PHP 8.1–8.5 with GCC and Clang,
all five GCC coverage builds, and PHP 8.3 ASan/UBSan. Each run included both new
PHPTs, for 281 selected tests. Counts were consistent across compiler and
instrumentation variants:

| PHP | Passed | Skipped |
| --- | ---: | ---: |
| 8.0.30 | 265 | 16 |
| 8.1.34 | 268 | 13 |
| 8.2.33 | 268 | 13 |
| 8.3.33 | 268 | 13 |
| 8.4.24 | 274 | 7 |
| 8.5.9 | 274 | 7 |

The following final checks also selected all 281 PHPTs:

| Linux x86-64 check | Passed | Skipped |
| --- | ---: | ---: |
| PHP 8.3.33, reconfigured workspace build | 268 | 13 |
| PHP 8.3.33, archive bridge with GCC | 274 | 7 |
| PHP 8.3.33, archive bridge with Clang | 274 | 7 |
| PHP 8.3.33, archive bridge with ASan/UBSan | 274 | 7 |
| PHP 8.3.33, Valgrind package, normal and memory-checking runs | 268 | 13 |

Every completed suite reported zero failures and warnings. The rebuilt
sanitized libmustache dependency passed its 22 upstream tests, including
`test_lambda_results` and `test_section_source`. The existing native boundary
prototype also rebuilt and passed its 22 rendering cases and terminal check.
It remains a simulated-abort control and does not resolve F7.

Final builds used a source snapshot containing tracked working-tree files and
both new PHPTs, without staging changes. Only this report's verification notes
changed after that snapshot. The manifest contains all 281 PHPTs exactly once,
and all 311 listed files exist. Stub/hash, local file links, and diff checks
passed, along with all eight configured pre-commit hooks.

```sh
nix flake update libmustache
nix flake check path:/tmp/php-mustache-c43ad03-final-lzs37p0g \
  --keep-going --no-write-lock-file -L --max-jobs 4 --cores 4
nix develop --no-write-lock-file --command bash -c \
  './configure --enable-mustache && make -B -j4'
nix build --no-link --keep-going --no-write-lock-file --max-jobs 4 --cores 4 -L \
  path:/tmp/php-mustache-c43ad03-final-lzs37p0g#php83-archive-benchmark \
  path:/tmp/php-mustache-c43ad03-final-lzs37p0g#php83-archive-benchmark-clang \
  path:/tmp/php-mustache-c43ad03-final-lzs37p0g#php83-archive-benchmark-sanitized \
  path:/tmp/php-mustache-c43ad03-final-lzs37p0g#php83-gcc-valgrind
```

## Archive benchmark

The full benchmark ran with PHP 8.3.33, APCu 5.1.28, 101 timed samples after
10 warmups, and 31 fresh-process samples for each workload. All six workloads
completed. The existing gate requires at least a 20% reduction in median and
p95 APCu cache-hit latency for each medium and large workload.

| Workload | Median reduction | p95 reduction | Gate |
| --- | ---: | ---: | --- |
| Medium flat | 60.48% | 59.31% | Pass |
| Medium partial graph | 55.12% | 59.80% | Pass |
| Large flat | 70.79% | 71.27% | Pass |
| Large partial graph | 67.36% | 71.23% | Pass |

These results compare archive caching with source caching in the updated build.
They do not measure performance changes between dependency revisions. Archive
payloads were 6.05–6.93 times the source graph's byte count on these fixtures.
The result metadata identifies `c43ad03` and was saved using:

```sh
BENCH_JSON=/tmp/php-mustache-c43ad03-benchmark.json \
  nix run --no-write-lock-file \
  path:/tmp/php-mustache-c43ad03-check-088elb6v#php83-archive-benchmark
```

## Remaining verification limits

Reliability verdict: **PASS_WITH_RESIDUAL_RISK**. The independent correctness
review raised the mixed-snapshot ABI limitation documented above. The independent
test review added archive callback coverage and demonstrated no defect with
matched snapshots. Final checks include that added test. The ABI limitation
remains documented; no production fix or new PHP result API is included.

All executed checks used Linux x86-64. Windows, macOS, ARM64, container images,
and the PIE installation workflow were not run. No remote CI run was triggered.
The new literal/template result controls have upstream C++ coverage, but no
PHP-level coverage because this update does not expose them to PHP callers.
