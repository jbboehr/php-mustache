# PHP lambda results: implementation proposal

Status: slice 1 implements the mode API and has passed review. The two dedicated
final result classes are selected for slice 2 and are not yet implemented.
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

Proposed stub excerpts; method bodies are omitted here:

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
proposed on either result class: implicit conversion would discard its explicit
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
classes describe slice 2. None of the new APIs exist in base commit `73587cf`.

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

The current [PHP callback bridge](../../mustache_lambda.cpp) always calls
`convert_to_string()` and implements the legacy string callbacks. Upstream's
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
new-type rejection in `mustache_data.cpp`. Add the sources to `config.m4` and
the package manifest. Extend the feature probe for the native result API when
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

The explicit result ownership design remains a proposal based on source
inspection. Examples using those classes and the combined stub excerpt have
only been syntax-checked. Slice 1's mode example was executed with the rebuilt
extension. PHPStan/IDE behavior is inferred from native types and PHPDoc; this
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
verification; it was not repeated here. The explicit result classes still
belong to slice 2, and the deferred Zend bailout work remains outside this
change. Slice 1 has passed review; the remaining implementation slices still
pause for review before committing.
