# Partial-map budget decision

The current implementation adds [opt-in entry and text limits](../php-api.md#partial-map-limits).
There is still no aggregate limit enabled by default. The original decision
below explains why no universal threshold or complete memory budget was chosen.

## Opt-in implementation

`Mustache::setPartialLimits()` replaces two per-instance settings. Both default
to `null`, with zero enforcing a zero allowance. Each render captures the
settings before calling PHP, then consumes its own remaining allowance during
partial preparation. Subtraction follows a size check, so accumulation does not
overflow. No counters are shared between nested or suspended renders.

Both partial preparation paths charge each supplied entry and its name. Source
strings and wrapper results are checked before their native source copy. ASTs
with retained source charge that source before reparsing with their saved
tokenizer settings. Binary-loaded ASTs charge the string fields and nested
partial names copied by the existing clone path, during its validation pass.
They retain the existing per-AST structural limits. The optional archive
serializer uses the same preparation helpers and captures its own limits.

The public contract measures input/prepared text, not allocator usage. Source
compilation can introduce string copies and nodes that are not included in this
text count. A complete aggregate node budget still needs a libmustache contract
for its opaque compiled handles. Applications choose their own entry/text limits,
so this change does not establish recommended default thresholds.

Focused tests cover inclusive boundaries, disabled and zero limits, rejected
configuration, repeated calls, representation-specific accounting, getter order,
nested calls, configuration changes, and Fiber suspension. These use small
inputs and caller-selected limits, not resource-exhaustion experiments.

### Implementation verification

The implementation follows base `1d7ee70` with libmustache pinned to
`e6b2de00d7a3ad801eb655165eed1fd9a4352b97`. All five new PHPTs were observed
failing with `missing setPartialLimits` against a pre-feature build, then passing
with the feature enabled. The archive test uses the optional benchmark bridge
and skips when that bridge is disabled. The Fiber test skips PHP 8.0.

An independent correctness review found no actionable defects. A separate test
review added the archive PHPT, covering entry/text boundaries, repeated calls,
unused entries, source and binary AST accounting, and callback snapshots. It
demonstrated no production failure. The final checks below include that test.

| Fresh Linux x86-64 build and full suite | Passed | Skipped |
| --- | ---: | ---: |
| PHP 8.0.30, GCC | 265 | 14 |
| PHP 8.3.33, workspace rebuild | 268 | 11 |
| PHP 8.4.24, GCC | 274 | 5 |
| PHP 8.3.33, ASan/UBSan with leak detection | 268 | 11 |
| PHP 8.3.33, archive benchmark enabled | 272 | 7 |

Every suite selected 279 tests and reported zero failures or warnings. The
sanitizer check reported no sanitizer errors. Additional archive controls
covered wrappers and repeated recovery after both kinds of limit rejection.
Native reflection matched the stub's public, non-static, nullable integer
parameters and `void` return type. The guide example printed `Hello Ada`.

Nix verification used a temporary source snapshot containing tracked working-tree
files and the five new PHPTs, without staging them:

```sh
nix build --no-link --keep-going --no-write-lock-file --max-jobs 2 --cores 4 -L \
  path:/tmp/php-mustache-partial-limits-final-w4eqb0u0#checks.x86_64-linux.php80-gcc \
  path:/tmp/php-mustache-partial-limits-final-w4eqb0u0#checks.x86_64-linux.php84-gcc \
  path:/tmp/php-mustache-partial-limits-final-w4eqb0u0#checks.x86_64-linux.php83-gcc-sanitized \
  path:/tmp/php-mustache-partial-limits-final-w4eqb0u0#php83-archive-benchmark
```

The workspace build used `nix develop --no-write-lock-file --command make -j4`.
Focused and full tests used the matching PHP 8.3 executable with
`run-tests.php -n -d extension=modules/mustache.so`, `REPORT_EXIT_STATUS=1`,
`NO_INTERACTION=1`, and matching `TEST_PHP_EXECUTABLE`. The full invocation added
`-j4 tests`.

Other PHP versions and platforms were not rerun. Binary AST delimiter and nested
partial-name charging was reviewed statically, without manufacturing binary
fixtures unavailable through ordinary public PHP construction. No aggregate
memory measurements or recommended default thresholds were established. The
separate Zend bailout ownership concern remains outside this implementation.

## Original default-limit decision

This was the I6 follow-up at `c194d91`, using libmustache revision
`fea4160d02238c7503d72e4f2705fda7b65edef5` from `flake.lock`.
It follows the [original recommendation](project-review-2026-09-04.md#i6-consider-a-budget-for-the-complete-partial-map).

**Decision at that revision: retain the current limits and defer a built-in aggregate budget.**
The repository fixtures do not establish appropriate defaults for applications
with many partials. No production workload distribution or process-memory target
was available for this slice. Reusing an existing per-template limit as a new
map-wide limit would change the accepted input contract without that evidence.
Adding configuration would also require a supported default and AST policy.

I6 remains optional hardening, not a demonstrated resource-exhaustion defect.
This decision does not establish that individual limits bound total map memory.
At that revision the PHP API exposed no aggregate partial-map budget. That
decision slice proposed no runtime behavior or public API changes.

## Existing enforcement and candidate accounting

At the reviewed revision, both preparation paths in
[mustache_mustache.cpp](../../mustache_mustache.cpp) visited supplied entries in
order, including unused ones. The source path compiles
each partial independently. The AST path tokenizes source entries and deep-clones
AST entries, starting a fresh `NodeCloneState` for each supplied AST. Source
parsing uses the dependency's defaults; cloning checks depth, node count, and
data-part counts. These are individual checks, not a shared map budget.

A future aggregate policy would need to distinguish these costs:

| Input | What needs accounting | When a check would matter |
| --- | --- | --- |
| Map entries and names | Entry count and total name bytes, including empty and unused partials | Before allocating native map entries and copying names |
| Source strings | Total supplied source bytes, counting each named entry | Before native source copies and compilation |
| `MustacheTemplate` values | The source actually returned by the wrapper, once per entry | After its source read, before copying and compiling that value |
| `MustacheAST` values | Nodes, data parts, and owned text copied by each clone, including nested partials | During bounded traversal, before the corresponding allocations |

The same AST supplied under two names is cloned twice by the current path.
Deduplicating an accounting pass by PHP object identity would not describe that
work. The [native node type](https://raw.githubusercontent.com/jbboehr/libmustache/fea4160d02238c7503d72e4f2705fda7b65edef5/src/node.hpp)
owns child nodes, nested partials, text, data-part strings, and delimiter strings.
Serialized byte length is not a measure of all clone allocations; serializing
an AST just to check a budget would also allocate before the check.

An entry-count and source-byte budget could be useful without being a complete
memory bound. Its documentation must say which costs it covers. A node budget
for source compilation would need an explicit accounting design across compiler
calls: passing the same [per-call limits](https://raw.githubusercontent.com/jbboehr/libmustache/fea4160d02238c7503d72e4f2705fda7b65edef5/src/mustache.hpp)
to every partial does not create a shared allowance. The root template, data
conversion, and rendering have separate limits and would remain distinct from
an I6 budget for partial preparation.

## Available workload evidence

The [archive benchmark](../../benchmarks/archive-cache-vs-source.php) defines
six source fixtures. Its `buildWorkload()` and `benchmarkData()` functions were
copied unchanged into a temporary script, without running the benchmark driver
or requiring its optional archive API. The script counted entries and byte
lengths and compared rendering with source, wrapper, and AST partials.

| Fixture | Partial entries | Root source bytes | Partial source bytes | Partial name bytes |
| --- | ---: | ---: | ---: | ---: |
| small-flat | 0 | 1,211 | 0 | 0 |
| small-graph | 3 | 89 | 1,092 | 15 |
| medium-flat | 0 | 33,047 | 0 | 0 |
| medium-graph | 3 | 89 | 32,942 | 15 |
| large-flat | 0 | 262,342 | 0 | 0 |
| large-graph | 3 | 89 | 262,067 | 15 |

For every fixture, five combinations produced byte-identical output:
source/source, source/wrapper, AST/source, source/AST, and AST/AST, with the root
representation first. The source result also contained the expected product
title. All 30 comparisons passed on PHP 8.3.33. Fifteen used nonempty maps; the
flat fixtures cannot establish partial-representation coverage.

These fixtures exercise source size and template structure, but never more than
three partials. They cannot justify an entry-count default or characterize
applications that load a large template catalog. Byte counts and matching output
are not latency, peak native-memory, or concurrency measurements. Passing these
fixtures is insufficient evidence for choosing a new rejection threshold.

## Source reads and error ordering

A complete preflight that resolves every wrapper before compiling any partial
could change which PHP code executes before an error. A separate sizing pass
that reads wrappers again could also use different source values. Any future
implementation must either preserve source-read and error-ordering behavior or
explicitly justify the change. The current behavior is observable with this
small control:

```php
<?php
class ObservedPartial extends MustacheTemplate
{
    public int $reads = 0;
    private string $source;

    public function __construct(string $source)
    {
        unset($this->template);
        $this->source = $source;
    }

    public function __get($name)
    {
        ++$this->reads;
        return $this->source;
    }
}

$mustache = new Mustache();
foreach (['source', 'AST'] as $backend) {
    $root = $backend === 'AST' ? $mustache->parse('{{>used}}') : '{{>used}}';
    $used = new ObservedPartial('{{name}}');
    $unused = new ObservedPartial('unused');
    $output = $mustache->render($root, ['name' => 'Ada'], ['used' => $used, 'unused' => $unused]);
    echo "$backend valid=$output reads=$used->reads/$unused->reads\n";

    $later = new ObservedPartial('unused');
    try {
        $mustache->render($root, [], ['bad' => '{{#missing}}', 'later' => $later]);
        throw new RuntimeException('Expected parser exception');
    } catch (MustacheParserException $error) {
        echo "$backend invalid=", get_class($error), " later_reads=$later->reads\n";
    }
}
```

The observed PHP 8.3.33 output was:

```text
source valid=Ada reads=1/1
source invalid=MustacheParserException later_reads=0
AST valid=Ada reads=1/1
AST invalid=MustacheParserException later_reads=0
```

Both paths read unused wrappers when preparing a valid map. An earlier invalid
source stops preparation before the later getter. The control only counts reads
and returns ordinary values; it does not mutate the map or test resource limits.

## Criteria for default limits or a complete budget

Revisit default thresholds and complete aggregate accounting when there is a
supported workload or deployment requirement to
choose limits against: partial counts, total names and source bytes, AST usage,
and an acceptable process-memory budget. Include a catalog-shaped workload
with many small entries as well as the existing few-entry fixtures.

Then define the supported representations, accounting units, failure behavior,
and configuration scope before adding enforcement. Check remaining allowance
before the corresponding allocation and use arithmetic that cannot overflow.
Normal-sized boundary tests should cover aggregate contributions, unused
entries, repeated ASTs, both render paths, getter ordering, cleanup, and recovery.
No new setter, INI option, or default threshold is selected in this slice.

## Verification and limits

Fresh Linux x86-64 checks used PHP 8.3.33 and the unchanged workspace extension:

- The six benchmark-fixture inventories and all 30 render comparisons passed.
  The table above was also checked against the recorded experimental results.
- The PHP example was extracted from this document and run with the extension
  loaded. Its output matched exactly, with exit status zero and no stderr.
- Five existing PHPTs passed: [source/AST equivalence](../../tests/Mustache__render-source-and-AST-partials-equally.phpt),
  [AST partials](../../tests/Mustache__render-partial-accepts-MustacheAST.phpt),
  [wrapper partials](../../tests/Mustache__render-partial-accepts-MustacheTemplate.phpt),
  [invalid inputs](../../tests/Mustache__rejects-invalid-templates-and-partials.phpt),
  and [source exception cleanup](../../tests/Mustache__template-source-exceptions-release-values.phpt).
- The full suite passed: 254 passed, 10 skipped, no failures or warnings.
  Runs used `REPORT_EXIT_STATUS=1`, `NO_INTERACTION=1`, the matching
  `TEST_PHP_EXECUTABLE`, and
  `run-tests.php -n -d extension=modules/mustache.so -j4 tests`.
- All configured pre-commit checks, an explicit Markdown check of the new
  document, 50 local links across the two changed documents, and manifest
  validation passed. All 264 PHPTs remain listed; maintainer documents remain
  excluded from the package.

This was a documentation and policy slice. No native rebuild, new PHPT, resource
exhaustion experiment, peak process-memory measurement, sanitizer run, other PHP
version, or other platform was tested. The archive benchmark's timing, APCu,
and fresh-process phases were not run. The observed behavior does not validate
an aggregate-budget implementation or establish suitable default thresholds.
