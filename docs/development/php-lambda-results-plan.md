# PHP lambda results: implementation proposal

Status: all three slices are complete and have passed review: the mode API,
two dedicated final result classes, compatibility verification, and the local
[issue-resolution draft](issue-68-resolution.md).
The base is dependency-update commit `73587cf`. The dependency is
libmustache `c43ad034850bab310d754bc3bba760cc31b08ef4`; its
[update report](libmustache-update-2026-09-07.md) records verification and
the requirement to rebuild the library and extension together.

Scope: the PHP API and binding for callback result interpretation. The primary
caller is a PHP application developer returning computed text from closures or
object methods, using the shipped stub and examples on PHP 8.0 or later.

[Issue 68](https://github.com/jbboehr/php-mustache/issues/68) asks for callback
output to be displayed without parsing its contents as another template.
The proposed binding supports that choice for an entire `Mustache` instance
and explicitly for an individual callback result.

## Recommended public API

Public API excerpts; method bodies are omitted here:

```php
class Mustache
{
    /** @var int */
    public const LAMBDA_STRING_TEMPLATE = 0;
    /** @var int */
    public const LAMBDA_STRING_LITERAL = 1;

    /** @phpstan-return self::LAMBDA_STRING_TEMPLATE|self::LAMBDA_STRING_LITERAL */
    public function getLambdaStringMode(): int {}

    /**
     * @phpstan-param self::LAMBDA_STRING_TEMPLATE|self::LAMBDA_STRING_LITERAL $mode
     * @throws ValueError If the mode is unknown.
     */
    public function setLambdaStringMode(int $mode): void {}
}

final class MustacheLiteralResult
{
    public function __construct(string $text) {}

    public function getText(): string {}
}

final class MustacheTemplateResult
{
    public function __construct(string $text) {}

    public function getText(): string {}
}
```

Use named integer constants to retain the project's PHP 8.0 baseline;
[native PHP enums require PHP 8.1](https://www.php.net/manual/en/language.enumerations.overview.php).
The setter follows normal PHP integer-argument rules, including weak coercion
where PHP permits it. Validate the resulting integer against these two values
before changing configuration. Unknown values throw `ValueError`; unsupported
argument types follow PHP's `TypeError` behavior. Invalid calls leave the
previous setting intact. The parameter names `mode` and `text` are part of
the named-argument contract.

The concrete class selects interpretation, and each constructor requires the
text to return. Both classes expose `getText()` for testing and inspection;
the class itself identifies the interpretation. No mode getter, factory methods,
shared public base class, or interface is needed. A callback that returns any
of the supported forms can declare
`string|MustacheLiteralResult|MustacheTemplateResult` as its return type.
Ordinary PHP strings express upstream's `Inherit` result. No `__toString()` is
provided on either result class: implicit conversion would discard its explicit
interpretation.

### Why these are dedicated classes

`MustacheTemplate` keeps its existing source-wrapper contract. It is mutable
and subclassable, and the root/partial path reads its `template` property while
callback-return conversion uses `__toString()`. An uninitialized wrapper is
also rejected as a root template but currently converts to an empty callback
return. Reusing it would require choosing which of those behaviors defines
an explicit result.

The new result classes instead have one immutable text value and a fixed
interpretation. `MustacheTemplateResult` is recognized as a callback return;
the accepted root and partial types remain those in the
[PHP API guide](../php-api.md#templates-and-partials). Returning an existing
`MustacheTemplate` from a callback continues through ordinary string conversion
and follows the engine's string mode.

### Default for slice 1

The first implementation keeps `LAMBDA_STRING_TEMPLATE` as the default and
makes literal mode opt-in. This preserves existing callback
behavior while exposing the requested control. It also matches the pinned
upstream default.

The earlier preference for literal-by-default remains a separate release
decision. We can choose it before release, using the same API, but it
changes existing callbacks that intentionally return template source:

| Callback returns | Template default | Literal default |
| --- | --- | --- |
| `'Hello {{name}}'`, with `name = 'Ada'` | `Hello Ada` | `Hello {{name}}` |
| `new MustacheLiteralResult('Hello {{name}}')` | `Hello {{name}}` | `Hello {{name}}` |
| `new MustacheTemplateResult('Hello {{name}}')` | `Hello Ada` | `Hello Ada` |

If the release adopts a literal default, document the change and show either
`setLambdaStringMode(Mustache::LAMBDA_STRING_TEMPLATE)` for existing applications
or explicit template results for selected callbacks. Keep the Mustache lambda
specification tests running in template mode as a compatibility check.
Do not silently rewrite their expectations to accept a different default.

## Proposed consumer examples

The ordinary-string example is implemented in slice 1. Examples using result
classes are implemented in slice 2. None of these APIs exist in base commit
`73587cf`.

### Ordinary callbacks returning text

```php
<?php
$mustache = new Mustache();
$mustache->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);

echo $mustache->render('{{value}}', [
    'name' => 'Ada',
    'value' => fn (): string => 'Hello {{name}}',
]);
// Hello {{name}}
```

The setting applies to plain callback returns after the binding's existing
string conversion. Existing scalar and stringable-object conversions retain
their current PHP behavior. A callback can override literal mode with
`return new MustacheTemplateResult('Hello {{name}}');`.

### Returning text already rendered by a section helper

```php
<?php
$mustache = new Mustache();
echo $mustache->render('{{#keep}}{{name}}{{/keep}}', [
    'name' => '{{other}}',
    'other' => 'Ada',
    'keep' => function (string $text, MustacheLambdaHelper $helper): MustacheLiteralResult {
        return new MustacheLiteralResult($helper->render($text));
    },
]);
// {{other}}
```

The helper renders once in the section's data and partial context. The explicit
literal result preserves those output bytes under either engine mode. Returning
the helper's plain string under today's template mode instead produces `Ada`
because that intermediate output is parsed again.

### Literal output and HTML escaping

```php
<?php
$mustache = new Mustache();
echo $mustache->render('{{value}}', [
    'value' => fn (): MustacheLiteralResult =>
        new MustacheLiteralResult('<b>{{name}}</b>'),
]);
// &lt;b&gt;{{name}}&lt;/b&gt;
```

Literal means that returned text bypasses template parsing. Double-brace
interpolation still escapes HTML according to the template's flags; triple
braces and ampersand tags retain their unescaped behavior. Section results
receive no additional section-wide escaping. Neither result class marks text as
HTML-safe. Returning an explicit template result retains upstream's existing
escaping of the complete evaluated interpolation.

## Configuration and result contracts

- Store the string mode per PHP engine instance and initialize it during native
  object creation, including for subclasses that bypass the parent constructor.
  No INI setting or additional `render()` parameter is proposed.
- Each render captures its mode before data conversion, root/partial property
  reads, or callbacks can execute PHP. Changing the configured mode affects
  subsequent calls, including a nested call started afterward. It does not
  change an active or suspended render. The getter reports the configured value
  for subsequent calls, which can differ from an active render's snapshot.
- Rendering through a section helper uses the active render's snapshot. Keep
  existing helper argument counts, delimiter handling, exception propagation,
  and callback-scoped lifetime. Helper rendering continues to return a string;
  callers choose whether to wrap it.
- Apply the mode at rendering time to source strings, `MustacheTemplate`, AST
  roots and partials, and the optional archive bridge. A parsed or archived
  template does not capture it. Reusing `MustacheData` must work under either
  mode because results are interpreted when its callbacks run.
- Preserve the existing overlap contract: independent source renders can
  overlap; overlapping AST renders on one engine remain rejected. Successful
  mode changes during an AST callback must not mutate its active renderer.
- Make both result classes final and immutable, with owned, length-aware text.
  Their class determines their interpretation. They retain no engine, callback,
  or helper. Callers may retain and return the same result repeatedly after a
  render ends. Allow cloning with identical text and class; `==` compares those
  two values, while `===` retains PHP object identity. Reject dynamic properties
  and use of uninitialized instances. Reject calling a constructor again with
  `Error` and retain the original text. Initially reject PHP serialization;
  callers can persist text and reconstruct the intended class with `new`.
- Accept a result object specifically as a callback return. Reject it in
  ordinary input data with `ValueError`, explaining that it belongs in a
  callback return. This prevents its inspection methods from accidentally
  becoming ordinary object-method lambdas during data conversion.

Template result interpretation follows the pinned
[upstream contract](https://github.com/jbboehr/libmustache/blob/c43ad034850bab310d754bc3bba760cc31b08ef4/docs/development/abi-6-source-migration-2026-08-20.md#migrating-lambdas):
interpolation uses default delimiters, while sections use their opening
delimiters. Literal text still consumes output budget; escaping can increase
that charge. It incurs no returned-template parsing charge. Section input
continues to consume its existing lambda text allowance.

## Binding design

Before slice 2, the [PHP callback bridge](../../mustache_lambda.cpp) always called
`convert_to_string()` and implemented the legacy string callbacks. Upstream's
`invokeResult()` adapter wraps those strings as `Inherit`, which explains why
the dependency update needed no PHP source changes.

For explicit results, implement both `invokeResult()` overrides in the PHP
`Lambda` subclass. Share callback invocation and result conversion between
interpolation and sections. Inspect the returned value, including reference
returns, for either result class before any string coercion. Copy its text into
the corresponding owning C++ result while the PHP value is retained:

| PHP return value | Native result |
| --- | --- |
| `MustacheLiteralResult` | `mustache::LambdaResult::literal(text)` |
| `MustacheTemplateResult` | `mustache::LambdaResult::templateSource(text)` |
| Ordinary string or other existing convertible return | `mustache::LambdaResult::fromString(text)` after existing conversion |

Keep `ZvalGuard` and argument ownership through exceptions. Preserve an existing
PHP exception from the callback or `__toString()` and propagate parser failures
through the existing exception translation. Remove superseded string-only
overrides after checking for direct callers. This is not a change to callback
discovery, argument arity, or ordinary PHP return coercion.

Share a small native payload containing a `zend_string*` and common text
ownership helpers between the two final classes. Derive interpretation from
the class entry; a separate mutable mode field is unnecessary. Initialize
storage safely before publication, fill it once during construction, release
the string in the object destructor, and copy its reference when cloning.
Provide comparison handling for class and native text, including the case of
identical text in different result classes. Keep the payload inaccessible to
property mutation. Follow the existing PHP-version-specific serialization-denial
patterns, and test constructor reentry and construction through reflection.

For mode configuration, add a field to `php_obj_Mustache` and have the PHP
setter update that field. A direct call to the upstream setter is insufficient:
the [current render paths](../../mustache_mustache.cpp) use independent native
renderers for compiled input but a shared renderer for AST input. Upstream
rejects changing the latter while rendering.

After capturing the PHP field at method entry:

1. Pass the captured mode to upstream's free compiled-render overload, with
   the same default render limits and prepared partial map.
2. For AST input, set the shared renderer's mode immediately before its existing
   render call, with no PHP execution between those operations. Upstream's
   active-render guard preserves the existing overlap rejection.
3. Pass the captured mode to the free archived-render overload in the optional
   benchmark method.

This avoids temporarily changing and restoring owner configuration across PHP
callbacks or Fiber suspension. The existing per-call partial budget is the
model for when to capture settings. Tokenizer configuration remains on its
existing path.

## Reviewable implementation slices

Each slice includes its own focused tests, stub/arginfo updates, user guide and
changelog entries, and manifest changes where applicable. Pause for review
before committing each implementation slice.

### 1. Expose ordinary-string mode

Add the constants, getter, setter, per-call snapshot, and propagation through
all three render paths. This gives applications an opt-in solution to issue 68
without changing callbacks. Extend the existing configure feature probe to
require the upstream mode API with a clear error for older development headers;
the unchanged library version alone does not identify that capability.

Main files: `mustache_mustache.cpp`, `mustache_mustache.hpp`, `config.m4`,
`mustache.stub.php`, and generated `mustache_arginfo.h`.

Tests cover default and switched modes; interpolation and sections; escaping;
invalid settings leaving state unchanged; named arguments and reflection;
source, wrapper, AST and archive paths; data/wrapper callbacks that change the
mode; nested calls, Fiber suspension, and recovery after an exception. Include
both AST roots and source roots forced onto the AST path by an AST partial.

### 2. Add explicit result objects

Add `MustacheLiteralResult` and `MustacheTemplateResult`, native registration,
object handlers, and callback result conversion. Register both classes in
`php_mustache.cpp` and include their declarations in the stub. Put their shared
implementation in `mustache_lambda_result.cpp`/`.hpp`; update
`mustache_lambda.cpp`/`.hpp` and the
new-type rejection in `mustache_data.cpp`. Add the sources to `config.m4`,
`config.w32`, and the package manifest. Extend the feature probe for the native result API when
it becomes a build requirement.

Tests cover both classes overriding both modes; closures, invokable objects,
and exposed object methods; callback returns by reference; empty text and
embedded NULs; source/AST/archive parity; custom delimiters; helper composition;
literal brace-containing text versus explicit template evaluation; preserved
legacy scalar/stringable conversion, including `MustacheTemplate` subclasses;
exceptions and later engine reuse. Test constructor and getter signatures,
finality, constructor bypass and repeated calls, immutability, cloning,
equality within and between classes, serialization rejection, direct-data
rejection, and retained/repeated results.
Use weak references and existing lifecycle patterns to check released values.

### 3. Finish compatibility and release verification

Run the full supported PHP/compiler checks on the combined implementation,
including the archive bridge, sanitizer build, and Valgrind check. Check the
public guide examples, generated stub hash, package manifest and all hooks.
Native reflection must match the proposed declarations on supported runtimes.

Document a migration example if a literal default was chosen, and confirm
that ordinary existing callbacks retain the tested compatibility path. Record
which runtimes and platforms actually ran. After review, prepare an issue-68
resolution description showing both usage styles; publishing or closing the
issue is a separate authorized action.

## Planning evidence

Before the dependency-update commit, fresh PHP 8.3.33 verification passed the
two archive tests and the normal full suite: 268 passed, 13 skipped, zero
failures or warnings. All eight repository hooks, explicit report Markdown
lint, and manifest validation also passed. The update report records the
earlier, broader matrix for that same runtime change.

During planning, a PHP 8.3.33 control verified that a plain callback returning
`<b>{{name}}</b>` evaluates `name` before interpolation escaping. Another
control verified that the helper example above produces intermediate
`{{other}}` and final `Ada` when it returns an ordinary string. Reflection
checks confirmed that the proposed mode method and result classes were absent
from the baseline extension.

Separate PHP 8.3.33 controls checked the `MustacheTemplate` reuse question.
A subclass with stored source `stored {{name}}` and `__toString()` returning
`converted {{name}}` rendered `stored Ada` as a root or partial, but
`converted Ada` as an interpolation or section callback return. An
uninitialized wrapper threw `ValueError` as a root and became an empty string
as a callback return. Constructor reinitialization replaced a wrapper's source,
and reflection confirmed that the class is not final. These results support
keeping the new result classes separate from the existing source wrapper.

During planning, the explicit result ownership design was based on source
inspection, and examples using those classes were only syntax-checked.
Slice 2 implements that design; its runtime evidence is recorded separately
below. Slice 1's mode example was executed with the rebuilt extension.
PHPStan/IDE behavior is inferred from native types and PHPDoc; this
repository has no configured PHPStan verification. Constant-union annotations
use `@phpstan-param` and `@phpstan-return` so PHP's stub generator retains the
native integer signatures.

For implementation, first observe focused tests fail for the expected missing
API or behavior, then make them pass. Run the repository reliability gate after
each non-trivial runtime slice and repeat final checks after any accepted fixes.
Use small ordinary fixtures for semantics and cleanup checks. New configurable
render budgets, archive-format changes, and the deferred F7 Zend bailout work
are outside this proposal.

## Slice 1 verification

All six initial PHPTs were observed failing against pre-feature modules with
`missing lambda string mode`. Five ran against the PHP 8.3 archive build; the
lazy-data test ran separately with PHP 8.4. Their skip conditions select the
required runtime or optional archive bridge, rather than skipping a missing
mode API. They then passed in matching feature builds.

The configure probe was exercised with the preceding libmustache development
headers as well as the new pin. The older headers passed the existing ABI 6
check, then failed the new mode check with the intended rebuild instruction.
The updated headers passed, including the optional archive overload check.

After initial verification, the independent correctness reviewer returned
`PASS` with no actionable production defect. The independent test reviewer
returned `HARDENED_NO_FAILURE`. Its retained change extends the Fiber test to
a source root with an AST partial: this uses the shared renderer, rejects an
overlapping AST render, and still retains the suspended render's original
mode. The partial emits visible text so the expected output also confirms
that it was rendered. The strengthened test passed on PHP 8.3 ordinary and
archive builds and PHP 8.4.

Final verification ran after both reviews, using a source snapshot that
included all six new tests. `nix flake check --keep-going` passed all 18
configured checks: 17 runtime configurations and the pre-commit check. The
three optional PHP 8.3 archive configurations also built and passed their full
suites. Results for each configuration in a row were identical:

| PHP | Configurations | Passed | Skipped |
| --- | --- | ---: | ---: |
| 8.0.30 | GCC | 268 | 19 |
| 8.1.34 | GCC, Clang, GCC coverage | 272 | 15 |
| 8.2.33 | GCC, Clang, GCC coverage | 272 | 15 |
| 8.3.33 | GCC, Clang, GCC coverage, GCC ASan/UBSan | 272 | 15 |
| 8.4.24 | GCC, Clang, GCC coverage | 279 | 8 |
| 8.5.9 | GCC, Clang, GCC coverage | 279 | 8 |
| 8.3.33 archive bridge | GCC, Clang, GCC ASan/UBSan | 279 | 8 |

Every runtime configuration selected 287 PHPTs, with zero failures and zero
test warnings. Skips reflect runtime-specific features and whether the optional
archive bridge is enabled. The final workspace PHP 8.3 run also passed its full
suite (272 passed, 15 skipped) and focused mode tests (four passed, two skipped).
The archive-mode test passed in archive builds; the lazy-data mode test passed
on PHP 8.4 and 8.5.

The final build commands used the snapshot
`/tmp/php-mustache-mode-final-x3rfzi7p`:

```sh
nix flake check path:/tmp/php-mustache-mode-final-x3rfzi7p \
  --keep-going --no-write-lock-file -L --max-jobs 4 --cores 4
nix build --no-link --keep-going --no-write-lock-file -L \
  --max-jobs 4 --cores 4 \
  path:/tmp/php-mustache-mode-final-x3rfzi7p#php83-archive-benchmark \
  path:/tmp/php-mustache-mode-final-x3rfzi7p#php83-archive-benchmark-clang \
  path:/tmp/php-mustache-mode-final-x3rfzi7p#php83-archive-benchmark-sanitized
```

The combined logs are `/tmp/php-mustache-mode-final-matrix.log` and
`/tmp/php-mustache-mode-final-archives.log`. These temporary paths identify the
local evidence; they are not repository artifacts. Only this plan's verification
notes were edited after the snapshot. Production sources, build configuration,
stubs, tests, and the package manifest remained identical to it.

All eight repository hooks passed. Explicit Markdown lint included this
plan before it was tracked, and local documentation links were checked. The generated stub
hash matches; all 317 manifest entries exist, with all 287 PHPTs represented
and no duplicate entries. The guide's ordinary-string example was executed and
returned `Hello {{name}}` as documented. Final diff checks passed.

Runtime verification was on x86_64 Linux. Other operating systems and CPU
architectures were not tested. Valgrind remains part of slice 3's combined
verification; it was not repeated for slice 1. The explicit result classes
belong to slice 2, and the deferred Zend bailout work remains outside this
change. Slice 1 has passed review; the remaining implementation slices still
pause for review before committing.

## Slice 2 verification

Slice 2 is based on mode-API commit `e87614a`. All seven initial PHPTs failed
against a loaded pre-feature PHP 8.3 archive module with
`missing explicit lambda results`. That includes the optional archive test;
the feature's absence was checked in the test body rather than its skip logic.

The first implementation passed the value, rendering, helper, callback, and
lifetime tests. The data-rejection test exposed an exception mismatch:
`InvalidParameterException` became `ValueError` in `render()`, but only a warning
in `MustacheData` construction and the diagnostic API. The new result-type guard
now raises `ValueError` directly and unwinds through the existing PHP-exception
path. The test then passed without changing its expected behavior. Existing
validation for other data types remains unchanged.

The PHP 8.0 build exposed a separate fixture issue: the value test used
`class_alias()` on an internal class, which PHP only supports
[from PHP 8.3 onward](https://www.php.net/manual/en/function.class-alias.php).
That runtime-specific check is separated from portable value semantics.

The four PHP-lambda examples in the public guide were executed with the rebuilt
PHP 8.3 extension. Their outputs were `Hello {{name}}`,
`Hello {{name}} | Hello Ada`, `ADA`, and `{{other}}`, respectively.

The independent correctness reviewer returned `PASS`, with no actionable
production defects. The independent test reviewer returned
`HARDENED_NO_FAILURE`. Retained test improvements verify the absence of a public
base or interface, rejection of result objects as root templates and partials,
and alias comparison and callback interpretation on PHP 8.3+. The new alias test
failed against the pre-feature module and passed with the new classes; it
correctly skips PHP versions that cannot alias internal classes. There were no
accepted production fixes from either review.

After both reviews, `nix flake check --keep-going` passed all 18 configured
checks: 17 runtime configurations and the pre-commit check. The three optional
PHP 8.3 archive configurations also passed their full suites. Results for each
configuration in a row were identical:

| PHP | Configurations | Passed | Skipped |
| --- | --- | ---: | ---: |
| 8.0.30 | GCC | 274 | 21 |
| 8.1.34 | GCC, Clang, GCC coverage | 278 | 17 |
| 8.2.33 | GCC, Clang, GCC coverage | 278 | 17 |
| 8.3.33 | GCC, Clang, GCC coverage, GCC ASan/UBSan | 279 | 16 |
| 8.4.24 | GCC, Clang, GCC coverage | 286 | 9 |
| 8.5.9 | GCC, Clang, GCC coverage | 286 | 9 |
| 8.3.33 archive bridge | GCC, Clang, GCC ASan/UBSan | 287 | 8 |

Each runtime configuration selected 295 PHPTs, with zero failures and zero test
warnings. The skips reflect runtime-specific features and whether the optional
archive bridge is enabled. The final workspace PHP 8.3 focused run passed seven
new tests and skipped the archive test; that test passed in all three archive
builds. Additional PHP 8.4 checks confirmed that `newLazyGhost()` and
`newLazyProxy()` reject both result classes with `Error`, so they cannot bypass
construction.

The extended configure probe passed with the pinned headers. A separate
configure run with the preceding development headers passed the old ABI 6
check, then failed the result API check with the intended rebuild instruction.
The log is `/tmp/php-mustache-results-old-headers.log`.

Final builds used a source snapshot containing all eight new PHPTs:

```sh
nix flake check path:/tmp/php-mustache-results-final-dpppgi11 \
  --keep-going --no-write-lock-file -L --max-jobs 4 --cores 4
nix build --no-link --keep-going --no-write-lock-file -L \
  --max-jobs 3 --cores 4 \
  path:/tmp/php-mustache-results-final-dpppgi11#php83-archive-benchmark \
  path:/tmp/php-mustache-results-final-dpppgi11#php83-archive-benchmark-clang \
  path:/tmp/php-mustache-results-final-dpppgi11#php83-archive-benchmark-sanitized
```

Logs are `/tmp/php-mustache-results-final-matrix.log`,
`/tmp/php-mustache-results-final-archives.log`, and
`/tmp/php-mustache-results-final-focused.log`. These temporary paths identify
local evidence rather than repository artifacts. Only this plan was edited
after the snapshot; the production sources, configuration,
stubs, tests, guide, changelog, and manifest remained identical to it.

All eight configured hooks passed. The stub hash matches the generated
arginfo, explicit Markdown and local-link checks passed, and all 327 package
manifest entries exist with no duplicates. All 295 PHPTs are represented in the
manifest. Final diff and new-file whitespace checks passed.

Verification was on x86_64 Linux. `config.w32` includes the new source file, but
Windows and other operating systems or CPU architectures were not tested.
Valgrind remains part of slice 3's combined verification and was not run here.
The default remains template evaluation; choosing a different release default
is still a separate decision. Slice 2 has passed review.

## Slice 3 verification

Verification used the combined implementation at `5b952be`. This slice changes
only this plan and the [issue 68 resolution draft](issue-68-resolution.md).

A fresh PHP 8.3.33 GCC build passed the full suite normally and under Valgrind
3.26.0 Memcheck. Both selected 295 PHPTs: 279 passed, 16 skipped, zero failures
and zero test warnings. The Valgrind run used `USE_ZEND_ALLOC=0` and the PHP test
runner's `-m` option and reported zero leaked tests. The optional archive bridge
was disabled for that run.

The full Nix checks and all three archive builds also passed:

```sh
nix flake check --keep-going --no-write-lock-file -L --max-jobs 4 --cores 4
nix build --no-link --keep-going --no-write-lock-file -L \
  --max-jobs 3 --cores 4 \
  .#php83-archive-benchmark \
  .#php83-archive-benchmark-clang \
  .#php83-archive-benchmark-sanitized
nix build --no-link --keep-going --no-write-lock-file -L \
  --max-jobs 2 --cores 4 .#php83-gcc-valgrind
```

The 17 runtime checks and three archive checks reused the successful slice 2
builds, including the ASan/UBSan builds. They were not fresh rebuilds in this
slice. All 17 runtime derivation and output paths match the slice 2 snapshot,
and all tracked files except this plan were byte-identical to that snapshot
before this slice's edits. The per-configuration counts in the slice 2 table
therefore remain the combined implementation's matrix results. The Nix
pre-commit check ran afresh and passed all eight hooks.

Additional fresh runtime checks used the GCC builds:

- The general arginfo, result-value, and string-mode PHPTs passed on PHP
  8.0.30, 8.1.34, 8.2.33, 8.3.33, 8.4.24, and 8.5.9. These check the public
  signatures, parameter names, result-class finality, and mode constants and
  defaults against native reflection and behavior.
- The PHP 8.3.33 archive build passed its full suite: 287 passed, eight skipped,
  zero failures and zero test warnings.
- The four PHP-lambda examples in the public guide and both examples in the
  issue draft were executed on all six PHP versions and the PHP 8.3 archive
  build. All 42 executions matched their documented output with no diagnostics.
  The draft's literal-mode example checks both escaped and unescaped HTML
  interpolation while preserving brace-containing callback text.

All eight local repository hooks also passed after the documentation edits.
Explicit Markdown lint included the untracked draft, and local documentation
links passed. The generated stub hash matches. All 327 package manifest entries
exist with no duplicates, and all 295 PHPTs are represented. Final diff and
new-file whitespace checks passed.

Local evidence is in `/tmp/php-mustache-lambda-results-valgrind.log`,
`/tmp/php-mustache-lambda-results-compatibility-matrix.log`,
`/tmp/php-mustache-lambda-results-compatibility-archives.log`, and
`/tmp/php-mustache-lambda-results-compatibility-focused.log`. These temporary
logs are not repository artifacts.

All runtime evidence is from x86_64 Linux. Windows and other operating systems
or CPU architectures remain unverified. Valgrind covered the ordinary build,
while the optional archive bridge has the separate sanitizer evidence above.
These runs do not establish safety across actual Zend bailouts. F7 and arena
work remain deferred.

Ordinary callback strings still default to template evaluation, so this feature
requires no default-change migration. Literal mode and the explicit classes are
opt-in. The local issue draft describes the unreleased APIs and has not been
posted. Slice 3 has passed review. Issue closure and release work remain
separate actions. The three implementation slices have no further code changes
planned.
