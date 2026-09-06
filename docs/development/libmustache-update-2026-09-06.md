# libmustache develop update: September 6, 2026

This update starts from php-mustache `2992324` and moves the libmustache pin
from `fea4160d02238c7503d72e4f2705fda7b65edef5` to
`e6b2de00d7a3ad801eb655165eed1fd9a4352b97`. The new revision matched
`refs/heads/develop` when refreshed during verification. The branch advanced
once during this work; the final checks below use `e6b2de0`.

The [23-commit upstream change](https://github.com/jbboehr/libmustache/compare/fea4160d02238c7503d72e4f2705fda7b65edef5...e6b2de00d7a3ad801eb655165eed1fd9a4352b97)
includes dotted-name lookup, lambda interpolation escaping, renderer ownership,
exact section callback text, legacy serialization validation, and build and
test changes. The reported library version remains 0.6.0.

The new upstream agent-badge input follows this project's existing input,
which already pins the same badge revision. Parsed lockfile comparison confirmed
that only the libmustache node changed. Linux, macOS, Docker, PIE, and Windows
build scripts already read its revision from `flake.lock`.

## Compatibility changes

Three new PHP integration tests passed with the updated dependency and failed
with the original dependency on PHP 8.4.24:

- [Dotted names](../../tests/Mustache__render-dotted-name-context-stack.phpt)
  now resolve path components through the context stack. Literal keys containing
  a dot no longer override a matching nested path or block parent-context lookup.
  Source, parsed AST, and binary AST rendering are covered.
- [Interpolation lambdas](../../tests/Mustache__render-escapes-complete-lambda-interpolation.phpt)
  now escape their complete evaluated result, including literal fragments and
  partial output. The test covers escaped and unescaped interpolation, source
  and AST partials, and all three template representations.
- [Custom-delimiter sections](../../tests/MustacheAST__rejects-lossy-custom-delimiter-serialization.phpt)
  remain renderable, but `toBinary()` and PHP `serialize()` throw
  `MustacheException` when the legacy format cannot retain their delimiters.
  The same AST remains reusable afterward. Cache template source for this case.
  Changing delimiters for interpolation alone does not trigger rejection.

The final upstream commit also preserves original `#` section callback text.
[Legacy serialization rejects bodies it cannot preserve](../../tests/MustacheAST__section-body-serialization.phpt),
including changes in tag spacing or spelling. Canonically spelled bodies still
round-trip through binary and PHP serialization. The PHP API guide, persistence
example, and stub exception documentation describe both restrictions.

## Demonstrated integration defect and correction

The [exact section-body regression](../../tests/Mustache__render-preserves-original-section-body.phpt)
exposed an integration defect after the dependency update: the extension's
AST-partial clone copied public node fields but omitted upstream's new private
source metadata. A callback received `A {{name}} B` from an AST partial where
the same source partial supplied `A {{ name }} B`. Spaced comments and
triple-brace spelling were also normalized. Source roots, direct AST roots,
and source partials preserved the bytes.

The test failed before the correction and passed afterward. Source-parsed ASTs
now retain their source and parse-time tokenizer settings. Copying one into a
partial map validates the existing tree and tokenizes that snapshot into an
independent owned tree. Binary-decoded ASTs retain the public-field clone path.
Both paths retain the existing depth, node, and data-part limits.

The [parser-settings regression](../../tests/Mustache__render-AST-partial-preserves-parser-settings.phpt)
also failed before the correction and passed afterward. It checks a source
getter that changes parser settings, exactly one source read, later source
changes, rendering with another `Mustache` instance, embedded NUL bytes, empty
AST partials, and unaffected settings for subsequent source partials.

Each source-parsed AST now stores an additional source string and a tokenizer
settings snapshot. Each use as a partial adds validation and reparsing work.
This uses the supported library API without accessing private fields. No
performance claim is made for AST partials; the archive benchmark below
measures a different path.

## Verification

All 17 runtime checks in the x86-64 Linux Nix matrix passed: PHP 8.0 with GCC,
PHP 8.1–8.5 with GCC and Clang, all five GCC coverage builds, and PHP 8.3
ASan/UBSan. Each suite contained 273 PHPTs, including all six new tests.
Counts were consistent across compiler and instrumentation variants.

| PHP | Passed | Skipped | Failed |
| --- | ---: | ---: | ---: |
| 8.0.30 | 261 | 12 | 0 |
| 8.1.34 | 263 | 10 | 0 |
| 8.2.33 | 263 | 10 | 0 |
| 8.3.33 | 263 | 10 | 0 |
| 8.4.24 | 269 | 4 | 0 |
| 8.5.9 | 269 | 4 | 0 |

The optional PHP 8.3 archive suites passed with GCC, Clang, and ASan/UBSan:
266 passed and seven skipped each. The Valgrind package passed both its normal
and memory-checking runs, with 263 passed and ten skipped each.

The workspace extension was reconfigured and rebuilt against the updated
libmustache output. Its full PHP 8.3 suite passed with 263 tests and ten skips.
The native bailout-boundary prototype rebuilt and passed all 22 rendering
cases and its terminal-state check.

The existing seven-file fuzz corpus replayed under ASan/UBSan with `-runs=0`,
reporting eight executions including initialization. No mutation campaign ran.
The package manifest contains all 273 PHPTs exactly once, and all 303 listed
files exist. Stub/hash verification and diff checks passed. All eight configured pre-commit
hooks passed, completing all 18 Nix checks. All nine local file links in the
changed Markdown files resolve.

Nix verification used a temporary source snapshot containing the tracked
working-tree files and accepted untracked files. This includes the new tests
without staging them. File hashes were compared against the checkout; only
this report changed after the runtime builds.

Commands run from the project root, using that snapshot:

```sh
nix flake update libmustache
nix flake check path:/tmp/php-mustache-e6b-final \
  --keep-going --no-write-lock-file -L --max-jobs 4 --cores 4
nix build --no-link --keep-going --no-write-lock-file -L \
  path:/tmp/php-mustache-e6b-final#php83-archive-benchmark \
  path:/tmp/php-mustache-e6b-final#php83-archive-benchmark-clang \
  path:/tmp/php-mustache-e6b-final#php83-archive-benchmark-sanitized \
  path:/tmp/php-mustache-e6b-final#php83-gcc-valgrind
nix develop --command bash -c './configure --enable-mustache && make -B -j4'
nix develop --command bash docs/development/prototypes/bailout-boundary/run.sh
nix run --no-write-lock-file \
  path:/tmp/php-mustache-e6b-final#php83-clang-fuzzer -- -runs=0 fuzz/corpus
```

## Archive benchmark

The complete benchmark used PHP 8.3.33, APCu 5.1.28, 101 timed samples after ten
warmups, and 31 fresh-process samples for each workload. All six workloads
completed. The existing gate requires at least 20% lower median and p95 APCu
cache-hit latency for each medium and large workload.

| Workload | Median reduction | p95 reduction | Gate |
| --- | ---: | ---: | --- |
| Medium flat | 71.18% | 71.60% | Pass |
| Medium partial graph | 66.76% | 67.12% | Pass |
| Large flat | 74.08% | 74.31% | Pass |
| Large partial graph | 67.78% | 67.65% | Pass |

These percentages compare archive and source caching within the updated build.
They do not measure the speed difference between dependency revisions. The
complete local result, whose metadata identifies `e6b2de0`, was written with:

```sh
BENCH_JSON=/tmp/libmustache-update-e6b-benchmark.json \
  nix run --no-write-lock-file \
  path:/tmp/php-mustache-e6b-final#php83-archive-benchmark
```

## Independent review and remaining limits

Reliability verdict: **PASS_WITH_RESIDUAL_RISK**.

The independent code review identified the serialization documentation gap
and AST-partial metadata loss. The independent test review reproduced the
latter. After the correction, focused code review found no actionable
regressions, and independent retesting passed the three section-text/settings
tests. Additional normal-rendering checks preserved exact text across an
inline delimiter change with nested tags and verified canonical binary AST
partial fallback. No accepted static finding remains unresolved in this slice.

All executed checks used Linux x86-64. Nix omitted its incompatible ARM64
targets. Docker could not connect because its daemon socket was absent, so the
container matrix did not run. macOS, Windows, and the PIE installation workflow
were not run here. No remote CI run was triggered. Legacy binary ASTs and the
experimental generation-2 archive format do not retain original source text.

The native prototype models abort propagation. It does not exercise an actual
Zend bailout or resolve the deferred F7 ownership concern. No resource
exhaustion or lifetime-corruption experiment was performed.
