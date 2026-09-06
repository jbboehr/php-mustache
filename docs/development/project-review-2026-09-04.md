# Project review: September 4, 2026

This report covers revision `38ad627d404e25278f95f2c59142219b4f3f6e03`.
It records seven findings and six improvement areas from the project review.
The scope included the native extension, tests, fuzz harness, build configuration,
CI scripts, packaging, benchmarks, and documentation.

Experimental follow-up confirmed F2's retained reference and the four behavioral
defects F3–F6 on PHP 8.3.33, 8.4.24, and 8.5.9. It also confirmed several tooling
and API observations described below. F1's callback premise was observed, but
its lifetime-invalidation concern remains conditional. F7 remains a static
extension-cleanup concern. Memory corruption and persistent-worker memory growth
were not reproduced. Suggested changes below have not been applied.

Source links refer to the reviewed files. Line anchors describe the reviewed
revision and may move as fixes are made. PHP examples are independent scripts
that assume the extension is loaded. Proposed C++ and configuration excerpts
illustrate individual changes, not complete patches.

## Findings at a glance

| ID | Issue | Evidence | Suggested action |
| --- | --- | --- | --- |
| F1 | Borrowed values cross lazy-object initialization | Callback observed on PHP 8.4/8.5; invalidation unverified | Investigate the lifetime contract |
| F2 | Temporary template-property values are not released | Extra reference observed on PHP 8.3/8.4/8.5 | Fix cleanup |
| F3 | Subclass property defaults remain uninitialized | Observed on PHP 8.3/8.4/8.5 | Initialize declared properties |
| F4 | Rendering ignores a MustacheData subclass's native data | Observed on PHP 8.3/8.4/8.5 | Use inheritance-aware recognition |
| F5 | Empty template wrappers reject valid empty source | Observed on PHP 8.3/8.4/8.5 | Preserve empty strings consistently |
| F6 | AST accessors return null despite non-null return types | Observed on PHP 8.3/8.4/8.5 | Throw a consistent exception |
| F7 | Zend bailouts bypass C++ callback cleanup | Static cleanup concern | [Deferred pending ownership evidence](zend-bailout-adapter-contract.md#integration-gate-and-next-work) |

## F1. Potential lifetime invalidation during lazy-object initialization

Affected code: [property collection](../../mustache_data.cpp#L337),
[array traversal](../../mustache_data.cpp#L290), and
[object conversion](../../mustache_data.cpp#L422).

Data conversion keeps borrowed PHP values and container iterators while calling
an object's `get_properties` handler. On PHP 8.4 and later, the standard handler
can initialize a lazy object and execute PHP code. This behavior is visible in
[PHP's object handlers](https://github.com/php/php-src/blob/PHP-8.4/Zend/zend_object_handlers.h).

Conversion does not explicitly retain stable values across that boundary or
check for a pending PHP exception immediately afterward. If initialization
changes state that an active traversal depends on, conversion may resume with
invalid references. An invalid native access could affect worker stability.
Whether a particular application can reach that condition remains unverified.

**Experimental check:** On PHP 8.4.24 and 8.5.9, an ordinary lazy ghost was
initialized exactly once during rendering, both directly and when nested in an
array or object property. Each case rendered `Ada`; a subsequent direct property
read did not rerun initialization. An initializer throwing RuntimeException
preserved the original exception and message, and a later render on the same
Mustache instance returned `Grace` successfully.

These controls establish that conversion executes the initializer and handles
these ordinary cases correctly. They do not establish invalidation: the
initializer only populated its own property, and no source-container mutation
or stale-pointer access was attempted. The constructor-reentrancy observation
below also remains unverified.

Decide whether conversion should initialize lazy objects. If it should, preserve
the lifetime of values used after callbacks and stabilize traversed containers.
Account for references and object properties, not just ordinary array
copy-on-write behavior. Propagate a pending PHP exception before continuing
conversion. If initialization is intentionally unsupported, document and enforce
that policy.

There is a related unresolved publication concern in
[MustacheData construction](../../mustache_data.cpp#L568). The constructor checks
for an existing payload before conversion, then publishes the result without an
initialization-in-progress state or a second check. Include constructor
reentrancy in the same lifetime review. This observation is not a separately
confirmed defect.

Add version-gated coverage for ordinary lazy-object conversion, initializer
exceptions, and the chosen initialization policy on PHP 8.4 and 8.5. Existing
magic-accessor and ArrayObject tests do not exercise this boundary.

## F2. Temporary template-property values are not released

Affected code: [mustache_template_object_source](../../mustache_mustache.cpp#L243).

`zend_read_property` can return an owned temporary through its `rv` argument.
The source-reading helper uses a local `rv` but never destroys it, on either
successful string conversion or a validation exception. This matters for
supported template subclasses whose property access produces a temporary value.
Refcounted strings or objects can retain an extra reference, and temporary
objects can miss timely destruction.

The equivalent [MustacheTemplate::__toString implementation](../../mustache_template.cpp#L86)
already initializes and releases its temporary. The two entry points therefore
handle the same Zend ownership contract differently.

**Experimental check:** A template subclass's getter returned a dynamically
created string that remained owned by the fixture. Its reference count was
measured before and after one operation, after discarding the operation's
result. Each row used a fresh wrapper. PHP 8.3.33, 8.4.24, and 8.5.9 produced
the same results:

| Operation | Before | After | Retained references |
| --- | --- | --- | --- |
| String cast through `__toString()` (control) | 2 | 2 | 0 |
| Parse wrapper | 2 | 3 | 1 |
| Render wrapper | 2 | 3 | 1 |
| Wrapper partial with source rendering | 2 | 3 | 1 |
| Wrapper partial with AST rendering | 2 | 3 | 1 |

The observed extra reference supports the cleanup finding on successful string
reads. This bounded check measured reference ownership, not leak bytes or
persistent-worker growth. Object-destructor timing and invalid-value cleanup
paths were not experimentally verified.

Use scoped ownership for the source helper's temporary, following the existing
[ZvalGuard](../../mustache_lambda.cpp#L18) pattern. The required sequence is:

1. Initialize temporary storage to `UNDEF` before calling the property handler.
2. Propagate any pending PHP exception.
3. Validate and copy the source while its owner is still alive.
4. Release the temporary on every normal and exception path.
5. Preserve an exception raised during cleanup.

A short ownership helper can reduce duplication, but merely adding destruction
after the successful return-value copy would leave error paths uncovered.

Extend the existing
[temporary-property lifetime tests](../../tests/MustacheTemplate____toString-preserves-uninitialized-sentinel.phpt)
to parsing, rendering, and partial-source conversion. Verify ownership and
cleanup order as well as output. A benign throwing-getter check preserved the
original RuntimeException on PHP 8.3, so replacement of that exception is not
claimed here.

## F3. Custom allocators omit subclass property initialization

Affected code: [Mustache allocation](../../mustache_mustache.cpp#L105) and
[MustacheData allocation](../../mustache_data.cpp#L111).

Both custom allocators reserve property storage and call
`zend_object_std_init`, but omit `object_properties_init`. These classes permit
subclassing. As a result, normal subclasses do not receive declared property
defaults, and a typed property with a default can throw on its first read.

This example produced the output below on PHP 8.3.33, 8.4.24, and 8.5.9:

```php
<?php
class EngineContext extends Mustache
{
    public string $marker = 'ready';
}

class DataContext extends MustacheData
{
    public string $marker = 'ready';
}

foreach ([new EngineContext(), new DataContext([])] as $object) {
    try {
        echo $object->marker, "\n";
    } catch (Error $error) {
        echo get_class($object), ": uninitialized property\n";
    }
}
```

Observed output:

```text
EngineContext: uninitialized property
DataContext: uninitialized property
```

Both reads should produce `ready`. The missing initialization belongs after
standard object initialization, as already done by the MustacheAST allocator:

```cpp
zend_object_std_init(&intern->std, ce);
object_properties_init(&intern->std, ce);
intern->std.handlers = &Mustache_obj_handlers;
```

Apply the equivalent initialization to the other affected allocator. The
[helper allocator](../../mustache_lambda_helper.cpp#L58) has the same omission,
so resolve its subclassing contract consistently. A separate helper-subclass
check, with its own public constructor and the same typed default, also threw
Error on the first property read on all three PHP versions. That check concerns
subclass allocation; it does not imply that ordinary engine-created helpers
have unusable rendering state.

Add typed and untyped default-property tests. Keep the existing
[AST factory property-default test](../../tests/MustacheAST__fromBinary-initializes-subclass-properties.phpt)
as coverage of the corresponding invariant for that class.

## F4. Rendering ignores native data stored in a MustacheData subclass

Affected code: [mustache_parse_data_param](../../mustache_mustache.cpp#L176).

The helper recognizes only an exact MustacheData class entry. A subclass
inherits the native constructor and `toValue`, but rendering treats it as an
ordinary PHP object. Its public properties and methods replace the native
context the caller expected to render. This produces silent data loss.

```php
<?php
class NamedData extends MustacheData
{
}

$data = new NamedData(['name' => 'Ada']);
$mustache = new Mustache();

echo $data->toValue()['name'], "\n";
echo '[', $mustache->render('Hello {{name}}', $data), "]\n";
```

Observed output:

```text
Ada
[Hello ]
```

The expected second line is `[Hello Ada]`. The example produced the displayed
output on PHP 8.3.33, 8.4.24, and 8.5.9.

After normalizing references, recognize wrapper subclasses using the same
inheritance-aware approach used for template and AST wrappers:

```cpp
if (Z_TYPE_P(data) == IS_OBJECT &&
    instanceof_function(Z_OBJCE_P(data), MustacheData_ce_ptr)) {
  // Keep the initialized-payload check before borrowing native data.
}
```

The helper is used by normal rendering and the optional archive-render bridge.
Diagnostic conversion calls the converter directly and should retain its
documented behavior.

The [nested-wrapper rejection](../../mustache_data.cpp#L425) also checks exact
class identity. Keep the policy consistent: if nested MustacheData is rejected,
reject nested subclasses too. A separate check confirmed this inconsistency on
all three PHP versions: a nested base wrapper raised ValueError, whereas a
nested subclass was accepted and rendered an empty value. Add tests for direct
derived wrappers, uninitialized derived wrappers, and the chosen nested-wrapper
policy.

## F5. Explicitly empty template wrappers reject valid empty source

Affected code: [MustacheTemplate construction](../../mustache_template.cpp#L61),
[template-source validation](../../mustache_mustache.cpp#L276), and
[partial-source validation](../../mustache_mustache.cpp#L291).

The constructor treats a zero-length string as though no string was supplied
and leaves the template property at its null sentinel. Wrapping a valid empty
template therefore changes its behavior:

```php
<?php
$mustache = new Mustache();

echo 'source: ', json_encode($mustache->render('', [])), "\n";

try {
    echo 'wrapper: ', json_encode(
        $mustache->render(new MustacheTemplate(''), []),
    ), "\n";
} catch (ValueError $error) {
    echo "ValueError\n";
}
```

Observed output:

```text
source: ""
wrapper: ValueError
```

The wrapper should render an empty string as well. Empty optional partials have
the same consistency problem.

**Experimental check:** The example was reproduced on PHP 8.3.33, 8.4.24, and
8.5.9. Additional controls compared raw empty strings, explicitly empty wrapper
construction, and a subclass that directly assigned an empty string to the
protected source property. Raw strings parsed and rendered successfully, and
empty partials produced `AB` from `A{{>p}}B`. Both wrapper forms raised ValueError
in parsing, rendering, and partial preparation for source and AST rendering on
all three versions. The directly assigned property isolates the validation
guards from the constructor's separate omission.

Preserve explicitly supplied empty strings in the constructor:

```cpp
if (template_str != NULL) {
  zend_update_property_stringl(
      MustacheTemplate_ce_ptr, Z_OBJ_P(_this_zval), ZEND_STRL("template"),
      template_str, template_len);
}
```

Also revise both source-validation guards that independently reject empty
wrapper contents. Changing only the constructor will not resolve the problem.
Use null or non-string storage to identify an uninitialized wrapper, while
accepting an initialized empty string.

Test omitted/null construction separately from explicit empty construction.
Cover string conversion, parsing, and source/AST rendering with empty templates
and empty partials. Existing tests that reject uninitialized wrappers should
continue to pass.

## F6. AST accessors violate their declared return types on invalid state

Affected code: [toArray](../../mustache_ast.cpp#L370),
[toBinary](../../mustache_ast.cpp#L396), and the
[common exception handler](../../mustache_exceptions.cpp#L65).

The missing-state guards throw `InvalidParameterException`. The common handler
turns that exception into a warning, and the accessors finish without assigning
a return value. A caller can therefore receive null despite the declared array
or string return type.

```php
<?php
$ast = (new ReflectionClass(MustacheAST::class))
    ->newInstanceWithoutConstructor();

set_error_handler(static function (int $severity, string $message): bool {
    echo "warning\n";
    return true;
});

try {
    var_dump($ast->toBinary());
    var_dump($ast->toArray());
} finally {
    restore_error_handler();
}
```

Observed output:

```text
warning
NULL
warning
NULL
```

`__serialize()` already throws ValueError for equivalent invalid state.
The example's warnings and null returns were reproduced on PHP 8.3.33, 8.4.24,
and 8.5.9.
Reuse the existing error helper in the accessors:

```cpp
if (payload->state == NULL || payload->state->node == NULL) {
  mustache_ast_value_error("MustacheAST was not initialized properly");
}
```

Add accessor tests for uninitialized objects and failed-deserialization recovery.
Include the string-conversion alias when checking consistent failure behavior.
Keep the non-null return types and ensure successful recovery still works.

## F7. Zend fatal-error bailouts bypass native callback cleanup

Affected code: [Lambda::invokeUserFunctionAsString](../../mustache_lambda.cpp#L83)
and its [rendering callers](../../mustache_mustache.cpp#L730).

PHP callbacks execute while C++ automatic owners hold native data, compiled
templates, partial maps, and output storage. The code handles ordinary PHP
exceptions through `EG(exception)` checks and C++ exception unwinding.

Zend fatal-error bailout is a different mechanism. It uses a non-local jump
that bypasses C++ destructors. An enclosing `catch (...)` does not reclaim
native owners on that path. In a persistent worker, abandoned native
allocations may outlive the failed request. The relevant contracts appear in
[PHP's callback implementation](https://github.com/php/php-src/blob/PHP-8.3/Zend/zend_execute_API.c)
and [Zend's bailout definitions](https://github.com/php/php-src/blob/PHP-8.3/Zend/zend.h).

This remains a static cleanup concern in the extension. No fatal-bailout
experiment or persistent-worker memory measurement was performed. A standalone
C++ control observed destructor execution for normal scope exit and C++
exceptions, but not for a non-local jump on the local compiler. It is not
evidence of an extension leak: jumping across non-trivial destructors is outside
the defined C++ unwinding contract, and that control did not execute Zend.
Leak volume, repeated worker exhaustion, and memory corruption remain unverified.

The subsequent [ownership review and prototype](zend-bailout-ownership.md) and
[real adapter contract](zend-bailout-adapter-contract.md) narrow the implementation
requirements. The [feasibility decision](zend-bailout-adapter-contract.md#integration-gate-and-next-work)
defers F7's runtime implementation: PHP-reference disposition after an
interrupted release is still an integration gate. F7 is unresolved, with no
claim of an experimentally demonstrated extension leak or a completed fix.

Design a Zend bailout boundary that allows native ownership to be reclaimed
before bailout propagation resumes. Check all callback-capable operations,
including property reads and value-to-string conversion, when deciding where
that boundary belongs. Adding another C++ catch block does not address the
non-local jump. Cleanup that can itself invoke PHP also needs consideration.

After implementing the ownership strategy, verify cleanup using disposable
worker processes and keep ordinary exception-lifetime coverage. The current
passing exception tests establish behavior for C++ unwinding, not bailout.

## Other improvements

### I1. Select and verify the PHP version used by CI

[linux.sh](../../.github/scripts/linux.sh#L17) installs the matrix's PHP development
package, while [suite.sh](../../.github/scripts/suite.sh#L14) uses unversioned
`php`, `phpize`, and the default php-config. The selected build/runtime version
therefore depends on the runner's PATH and alternatives configuration.

**Experimental check:** An isolated fixture executed the real suite functions
with logging substitutes for build/runtime tools. With `PHP_VERSION=8.4` and an
explicit `TEST_PHP_EXECUTABLE` pointing to `php8.4`, sourcing suite.sh replaced
that executable with unversioned `php`. The build invoked unversioned `phpize`
and configure without `--with-php-config`; the test function invoked unversioned
`php`. Installation functions were not run. This confirms command selection and
override loss, but does not demonstrate a wrong-version GitHub-hosted CI run.

Make version selection explicit so a runner-image change cannot silently reduce
matrix coverage. For the Ubuntu jobs, the selection could follow this pattern:

```bash
PHP_EXECUTABLE="$(command -v "php${PHP_VERSION}")"
PHPIZE="$(command -v "phpize${PHP_VERSION}")"
PHP_CONFIG="$(command -v "php-config${PHP_VERSION}")"
export TEST_PHP_EXECUTABLE="$PHP_EXECUTABLE"

actual_version="$("$PHP_EXECUTABLE" -r 'echo PHP_MAJOR_VERSION, ".", PHP_MINOR_VERSION;')"
test "$actual_version" = "$PHP_VERSION"

"$PHPIZE"
./configure --enable-mustache \
    --with-php-config="$PHP_CONFIG" \
    --with-libmustache="$INSTALL_PREFIX"
```

Integrate this with both coverage and normal build branches. Preserve platform
selection in the macOS and Windows jobs. Assert the header/config version as
well as the runtime version, and retain any explicit executable override
instead of unconditionally replacing it in suite.sh.

### I2. Correct the Nix license metadata

[nix/derivation.nix](../../nix/derivation.nix#L95) declares BSD-3-Clause, but
[LICENSE.md](../../LICENSE.md), [composer.json](../../composer.json), and
[package.xml](../../package.xml) declare MIT. This produces inconsistent package
metadata for downstream users and tooling.

**Evaluation check:** `nix eval --json .#packages.x86_64-linux.default.meta.license.spdxId`
returned `"BSD-3-Clause"`. The mismatch reaches evaluated package metadata;
it is not just an unused declaration. The Nix field should match the project's
license:

```nix
license = licenses.mit;
```

Verify the evaluated package metadata after changing the declaration.

### I3. Document the public data and error contracts

The [README](../../README.md) explains installation and basic rendering, but
several behaviors are mostly discoverable in source and tests:

- Supported scalar and container types, including mixed numeric/associative-array rejection.
- Which object properties and methods become visible to templates.
- How MustacheData ownership works and why lambda conversion back to PHP is lossy.
- Parser, data, and serialization limits, including whether callers can configure them.
- Exceptions, uninitialized wrapper behavior, and delimiter/escaping defaults.

For example, document the difference between a list and an object-like context:

```php
<?php
$mustache = new Mustache();

echo $mustache->render('{{#items}}{{name}} {{/items}}', [
    'items' => [['name' => 'Ada'], ['name' => 'Lin']],
]);
```

This prints `Ada Lin` followed by a space. A PHP array combining numeric
and string keys is rejected by the current converter. Document that restriction
explicitly rather than leaving users to infer it from a conversion error.
Both the example output and mixed-key rejection with ValueError were checked on
PHP 8.3.33, 8.4.24, and 8.5.9. The remaining documentation recommendations are
source-review judgments, not claims of experimentally demonstrated defects.

Keep end-user contracts in README.md or linked usage documentation. Put build,
sanitizer, stub-regeneration, and spec-regeneration workflows in CONTRIBUTING.md
or docs/development/. Existing [cache examples](../../examples.md) already warn
that binary entries should be invalidated after libmustache upgrades.

### I4. Add tests for interactions between features

The existing suite has substantial specification and recovery coverage. The
findings above show that coverage of individual features does not always cover
their combinations. Prioritize the following additions:

| Area | Useful assertion |
| --- | --- |
| Subclassing | Declared defaults initialize, native data survives subtype normalization |
| Empty templates | Strings, wrappers, ASTs, and partials agree for initialized empty source |
| Property handlers | Source conversion releases temporaries on success and exceptions |
| PHP 8.4+ object features | Lazy initialization and property hooks follow a documented policy |
| Invalid AST state | Accessors throw consistently and allow valid recovery afterward |
| Callback errors | Throwing warning handlers preserve exceptions and stop subsequent work |
| Fibers | Suspension and resumption preserve the correct callback context |
| PHP serialization | Wrapper initialization, aliases, and lifecycle behavior remain correct |

These are test candidates, not claims that every row contains a demonstrated
bug. Version-gate tests for newer PHP features.

**Experimental check:** Two Fibers interleaved lambda callbacks on one Mustache
instance using source rendering. Both suspended and resumed, returning their
respective `Ada` and `Lin` contexts. With AST rendering, the second Fiber instead
raised `MustacheException: Renderer is already rendering`; the first resumed
successfully. A later render succeeded in both cases. This behavior was the same
on PHP 8.3.33, 8.4.24, and 8.5.9. Document and test the intended concurrency
policy; these controls did not demonstrate context corruption. Property hooks
and every proposed interaction in the table were not exhaustively exercised.

The [fuzz harness](../../fuzz/php_mustache_fuzzer.c) leaves AST serialization to
libmustache's fuzz targets, as described in [fuzz/README.md](../../fuzz/README.md).
Those targets cannot establish every Zend wrapper lifecycle invariant. Keep
focused PHP serialization coverage alongside native format coverage.

### I5. Reduce ownership duplication and developer-tool maintenance

The temporary-value omission in F2 is a concrete reason to share the small
existing zval ownership helper across relevant implementation files. Keep the
helper narrow, and preserve the distinction between ordinary C++ unwinding and
Zend bailout described in F7.

[generate-tests.php](../../generate-tests.php) contains mixed line endings and
does not check whether generated PHPT files were written successfully. Normalize
line endings to the repository's LF convention. Make write failures terminate
generation instead of leaving a partial result with a success exit status.

**Experimental check:** The real generator processed a one-test JSON fixture
under a temporary working directory on PHP 8.3.33. A normal output directory
produced a PHPT file and exited zero. Pointing only that output file at the
standard `/dev/full` failure sink produced a 502-byte write-failure notice
(`errno=28`, no space left on device), but the process still exited zero.
No valid test file was produced in the failure case. Byte inspection also found
52 CRLF line endings and 21 bare LF line endings in the generator.

A focused write check could look like this:

```php
$written = file_put_contents($outputPath, $output);
if ($written !== strlen($output)) {
    throw new RuntimeException('Could not write generated test: ' . $outputPath);
}
```

Here `$outputPath` is the generated PHPT filename and `$output` is its complete
contents.

Remove stale comments such as the boolean setter described as setStartSequence
in [mustache_mustache.cpp](../../mustache_mustache.cpp#L569). Prefer the stub as
the source of API signatures, and add a lightweight generated-arginfo consistency
check to prevent future drift. The stub hash matched at the reviewed revision.

These changes can be small and independent. A broad runtime rewrite for style
would add unnecessary review and regression risk.

### I6. Consider a budget for the complete partial map

[Source partial compilation](../../mustache_mustache.cpp#L391) and
[AST partial preparation](../../mustache_mustache.cpp#L417) apply individual
template or clone limits, but do not impose an explicit budget for the entire
map. A map can contain many individually acceptable entries, and native
allocations are not all charged to PHP's memory accounting.

Where applications accept large partial maps, consider preflight limits for
entry count and aggregate source bytes, plus an appropriate policy for AST
entries. Apply the same policy to both rendering paths. Avoid summing unchecked
sizes or defining a limit that is enforced only after the expensive allocation.

Choose thresholds from supported workloads and document them. Treat this as
optional resource hardening. No denial-of-service condition or suitable default
aggregate threshold was established by the review.

**Experimental check:** On PHP 8.3.33, 8.4.24, and 8.5.9, an invalid partial
raised MustacheParserException even when the root template did not reference it.
This confirms eager partial validation. It does not measure aggregate memory
use, prove a denial-of-service condition, or establish useful budget thresholds.

## Verification recorded during the review

These results describe the reviewed revision. They are not evidence that any
suggested fix has been implemented.

| Check | Result |
| --- | --- |
| Existing Nix development-environment build | Succeeded |
| Full PHP 8.3 PHPT suite | 241 passed, four optional benchmark tests skipped, zero failures |
| Clean Nix PHP 8.3 GCC build and test check | Succeeded with the same test counts |
| Fresh PHP 8.3 ASan/UBSan suite | 241 passed, four skipped, zero failures or reported sanitizer errors |
| Benchmark-enabled Nix build | 244 passed, one default-API-absence test skipped, zero failures |
| Configured lint checks | actionlint, Alejandra, Composer validation, markdownlint, and shellcheck passed |
| Package manifest | All 245 PHPT files listed, all listed files present |
| Generated arginfo | Stub hash matched mustache.stub.php |
| Source diff at review completion | No tracked source changes |

The sanitizer run enabled leak detection and disabled Zend allocation. Benchmark
timing and material-win thresholds were not evaluated. This initial review ran
PHP 8.3; the follow-up below also ran PHP 8.4 and 8.5. Windows, macOS, other PHP
versions, and a new fuzz campaign were not executed. Selected dependency
headers and official PHP source were checked for contracts. libmustache itself
was not comprehensively audited.

## Experimental follow-up: September 5, 2026 UTC

The follow-up used Linux x86-64 with PHP CLI 8.3.33, 8.4.24, and 8.5.9. Each
runtime loaded its matching extension with `-n -d extension=...`. PHP 8.3 used
the existing workspace module; PHP 8.4 and 8.5 used the corresponding Nix check
outputs. Those Nix outputs were already cached, so their retrieval was not a
fresh compilation. The focused scripts and full suites below were freshly run.

| Area | What was verified | Remaining limit |
| --- | --- | --- |
| F1 | Ordinary lazy initialization, original exception, and subsequent recovery on PHP 8.4/8.5 | Lifetime invalidation and constructor reentrancy unverified |
| F2 | One extra retained string reference for four source-reading paths on all three versions; cast control retained none | Invalid-value cleanup, object destruction, and worker growth unmeasured |
| F3 | Typed defaults fail for engine, data, and helper subclasses on all three versions | Proposed allocator fixes not applied or tested |
| F4 | Direct derived context loses native data; nested base and derived wrappers receive different treatment on all three versions | Uninitialized derived-wrapper and optional archive paths not separately tested |
| F5 | Empty-string controls succeed; both wrapper forms fail across parse/render/partial paths on all three versions | Proposed constructor and validation changes not applied |
| F6 | Both accessors warn and return null on all three versions | Proposed exception change and string-conversion alias not separately tested |
| F7 | Standalone compiler control only | Extension bailout cleanup and persistent-worker effects unverified |
| I1 | Real CI functions select unversioned tools and replace an explicit executable in a logging fixture | Actual hosted runner version selection unverified |
| I2 | Evaluated Nix metadata reports BSD-3-Clause, inconsistent with the project's MIT declarations | Metadata correction not applied |
| I3 | List example and mixed-key rejection checked on all three versions | Documentation quality remains a review judgment |
| I4 | Source Fiber interleaving, AST concurrency rejection, and recovery checked on all three versions | Remaining interaction rows are test candidates |
| I5 | Generator write failure still exits zero; mixed line endings counted | Ownership-helper design and style improvements are recommendations |
| I6 | Unused partials are validated eagerly on all three versions | Aggregate budgets and resource impact remain a static hardening recommendation |

All five complete PHP examples in this document were extracted and executed
independently on each runtime. Their output matched the report, with exit status
zero and no unexpected stderr. The example illustrating a proposed generator
write check is a patch fragment and was not counted as an executable example.

The existing PHPT suite was run separately for each runtime with
`REPORT_EXIT_STATUS=1`, `NO_INTERACTION=1`, and `TEST_PHP_EXECUTABLE` set to that
runtime's absolute path. Each invocation used the matching module:

```bash
"$PHP_EXECUTABLE" -n run-tests.php -n -d "extension=$MUSTACHE_MODULE" -j4 tests
```

| Runtime | Selected | Passed | Skipped | Warned / failed | Exit status |
| --- | --- | --- | --- | --- | --- |
| PHP 8.3.33 | 245 | 241 | 4 | 0 / 0 | 0 |
| PHP 8.4.24 | 245 | 241 | 4 | 0 / 0 | 0 |
| PHP 8.5.9 | 245 | 241 | 4 | 0 / 0 | 0 |

The four skips concern optional benchmarks. These passing suites coexist with
the focused observations above because the existing tests do not assert all of
those behaviors. Sanitizer results in the preceding section belong to the
initial PHP 8.3 run; the follow-up was not a new sanitizer campaign.

Raw commands, stdout/stderr, exit codes, runtime/module paths, and PHPT logs are
retained locally under
`.c-review-results/20260904T235313Z/verification-20260905/`. Temporary fixtures
were used for focused checks; no production implementation or committed tests
were changed. Markdown lint and local source-link checks were rerun after the
report update.

## Review coverage and limitations

The native review enumerated 23 C/C++ files, 161 units, and 3,346 source lines,
including generated configuration. All four location assignments and both
supplementary sweeps returned artifacts. Build scripts, packaging, benchmarks,
tests, and documentation were also reviewed outside that native ledger.

The coverage gate remains unverified. All 293 required rows were answered, but
only 275 satisfied the checker. Eighteen violations concerned parser site
accounting or findings associated with a related unit instead of the exact
ledger unit. Eleven files had degraded parses. No files were unreadable or
excluded. Thirty-six units had no machine-counted question sites, so their
reported manual review cannot be established by the ledger.

The ledger is a consistency check, not independent proof of coverage. Review
agents retained shell access. No independent false-positive or severity judge
ran. Raw severities were the original reviewers' assessments, and some static
findings may be wrong or out of scope.

The local review artifacts are under
`.c-review-results/20260904T235313Z/`. They include `PROJECT_REVIEW.md`,
`REPORT.md`, `findings.json`, `REPORT.sarif`, `units.json`, `ledger-gate.json`,
and the `parts/` and `assignments/` directories. They are not required to read
this report and were not committed as part of its preparation.

The assembled native report contains eight submitted findings plus two
unresolved pointers that its assembler promoted into LOW entries. This report
groups the two subclass-default findings into F3 and treats the pointers as
related observations in F1 and F4. The follow-up verified the nested subtype
behavior in F4; constructor reentrancy in F1 remains unresolved.
