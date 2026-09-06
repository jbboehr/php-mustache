# Data conversion ownership review

The F1 design review at revision `0e5503a` proposed retaining lazy-object
initialization with an explicit ownership contract. The implementation slice
based on `b506a28` now adds owned container captures. The original concern about
invalid native access has not been experimentally demonstrated. The constructor
review below recommends retaining the existing publication rules. Zend bailout
cleanup remains a separate follow-up.

## Current behavior and evidence

[DataConverter](../../mustache_data.cpp) now traverses owned entries. Before
this change, recursive conversion could call an object's `get_properties`
handler while iterating a borrowed outer container. In PHP 8.4.24, the
[standard handler](https://raw.githubusercontent.com/php/php-src/php-8.4.24/Zend/zend_object_handlers.h)
can enter lazy initialization. Its
[property-table builder](https://raw.githubusercontent.com/php/php-src/php-8.4.24/Zend/zend_object_handlers.c)
also creates indirect entries pointing into declared property storage.

The converter now checks for a pending PHP exception immediately after
`get_properties`. Ordinary lazy ghosts and proxies, initializer exceptions,
and subsequent renderer reuse have regression coverage. The original report's
missing-exception-check observation is therefore resolved.

`ActivePathGuard` still tracks original array, object, and reference identities
for cycle detection. `CapturedValue` owns the resolved value and retains its
original reference wrapper when present. This keeps the identity alive while
conversion reads the captured referent. Entry storage owns keys and values
independently of the original buckets and indirect property slots.

## Capture contract

The converter captures each container's immediate entries before recursively
converting its children. Each captured entry owns its key and resolved value.
Objects remain the same PHP objects, and their properties are collected when
conversion reaches them. This is a per-container capture, not an atomic copy of
the entire object graph. A nested object's later initialization can still affect objects
whose properties have not yet been collected.

The shared converter gives `MustacheData`, rendering, and `debugDataStructure()`
the same rules:

1. The resolved input owner survives property-handler calls, property collection,
   and method-lambda creation.
2. A pending exception is checked immediately after the handler returns. Visible
   property entries are captured before recursive child conversion runs PHP.
3. Indirect entries and reference referents are copied into owned storage.
   Reference wrappers are retained separately for cycle detection. Sibling
   aliases remain valid because active identities are tracked only along the
   current recursion path.
4. Entry and key-byte budgets are charged before allocating their capture
   storage. Node and depth limits remain enforced during recursive conversion.
5. Temporary ownership is released before reporting success. A pending PHP
   exception raised during cleanup stops conversion and takes precedence over
   translating a native validation error. C++ exception paths release the same
   temporary ownership. The root owner is released before a caller publishes
   the result.

The implementation keeps array ordering, mixed-key rejection, property visibility,
backing-value behavior for property hooks, property-over-method precedence,
and lambda retention. Capturing values invokes neither userland clone hooks nor
property getters. The Zend property API used for collection is unchanged.

Immediate values now become fixed when their container is captured. Later
object-property collection remains observable. Key validation and capture-budget
errors occur before child conversion. Captures coexist with ancestor captures
and the native result, adding temporary storage bounded by the entry and
key-byte budgets. These budgets do not bound total process memory. Throughput
and peak resident-memory costs have not been benchmarked in this slice.

## Constructor publication

The follow-up review at `afaba55` recommends keeping the existing constructor
state model. No affected caller was found in the repository's documented usage
or tests that warrants adding an initialization-in-progress flag. This is an
applicability decision, not a claim that overlapping constructor calls are safe.

[`MustacheData::__construct()`](../../mustache_data.cpp) rejects an existing
native payload, converts into a `unique_ptr`, and publishes only after conversion
and temporary-owner cleanup return successfully. The F1 capture change did not
introduce the constructor's check-before-conversion pattern.

| Entry path | Current behavior |
| --- | --- |
| Ordinary `new MustacheData($input)` | The constructor does not pass its instance to the input's callbacks. The `new` expression returns it only after construction finishes. |
| Repeated call on initialized data | The existing payload check rejects replacement before converting the new input. |
| Failed conversion with a nonthrowing warning handler | The `new` expression can return an uninitialized instance. It cannot be rendered or read as valid data. |
| Sequential retry on an uninitialized instance | A later direct constructor call can initialize it. Failed retries leave it uninitialized. |
| Subclass, reflection bypass, cloning, or PHP serialization | Finality and the wrapper's allocation, clone, and serialization restrictions reject these paths. |
| Lazy `MustacheData` wrapper on PHP 8.4 | PHP rejects both lazy ghosts and proxies for this internal class. Lazy objects supplied as input remain supported. |

The warning contract matters here. Native validation errors reach
[`mustache_exception_handler()`](../../mustache_exceptions.cpp) only after
conversion has unwound out of the constructor's `try` block. A warning handler
therefore runs after that failed conversion; the failed call does not later
resume conversion and publish a result. PHP exceptions from input callbacks
also stop publication. The [PHP API guide](../php-api.md#errors) documents the
uninitialized state left by a nonthrowing warning.

Finality and delayed publication narrow the concern but do not provide an
isolation boundary against arbitrary PHP callback code. There is still no guard
for overlapping direct constructor calls on the same uninitialized instance.
The documented construction and rendering flows do not require that behavior,
and scalar or container data alone does not invoke the constructor again. The
remaining concern depends on application PHP code manipulating the wrapper's
lifecycle during conversion; it has not been experimentally demonstrated.

Accept that limitation for this slice. Adding a flag now would introduce another
state and failure-reset rule without an established affected application flow.
Revisit the decision if a supported factory starts exposing instances before
initialization completes, or an application requires overlapping initialization.
Sequential retry is recorded as existing behavior, not proposed as a new API.

## Verification

### Capture implementation

The capture and key-validation tests first failed against the previous PHP
8.4.24 extension for the intended behavioral differences. The original 20
capture cases read `after` instead of `before`. The two later nested-array
cases also failed against that extension. All six key-validation cases ran
the child initializer and received its RuntimeException instead of ValueError.
The fixtures retain their containers and change existing scalar values only.

Fresh Linux x86-64 builds and full PHPT suites passed:

| PHP | Passed | Skipped | Failed |
| --- | ---: | ---: | ---: |
| 8.0.30 | 252 | 12 | 0 |
| 8.3.33 | 254 | 10 | 0 |
| 8.3.33, ASan/UBSan | 254 | 10 | 0 |
| 8.4.24 | 260 | 4 | 0 |
| 8.5.9 | 260 | 4 | 0 |

PHP 8.3 used the rebuilt workspace module. The other rows used fresh Nix check
outputs, followed by direct PHPT runs with the matching absolute PHP and module
paths. Build and lint commands were:

```sh
nix develop --command make -j4
nix build --no-link \
  .#checks.x86_64-linux.php80-gcc \
  .#checks.x86_64-linux.php83-gcc-sanitized \
  .#checks.x86_64-linux.php84-gcc \
  .#checks.x86_64-linux.php85-gcc
nix develop --command pre-commit run --all-files
```

PHPT runs used `REPORT_EXIT_STATUS=1`, `NO_INTERACTION=1`, and
`run-tests.php -n -d extension=MODULE tests`. The sanitizer run additionally
used the allocator and ASan/UBSan settings from `nix/derivation.nix`, with leak
detection enabled. All package-manifest and changed-document link checks passed.

Relevant passing coverage includes:

- [Captured values](../../tests/MustacheData__captures-container-values.phpt):
  maps, lists, and declared properties capture reference referents, while later
  objects expose their updated properties. Both lazy-object kinds and all four
  conversion entry points are covered.
- [Key validation](../../tests/MustacheData__validates-keys-before-child-conversion.phpt):
  mixed keys are rejected without running child initializers, including when
  template source is invalid.
- [Capture cleanup](../../tests/MustacheData__releases-captured-values.phpt):
  ordinary property objects and sibling aliases are released after successful
  and failed conversion. This is characterization coverage that also passes
  with the old extension.
- [PHP-exception cleanup](../../tests/MustacheData__releases-captures-after-php-exception.phpt):
  an initializer's original exception survives while sibling captures are
  released and later rendering succeeds. This also passes with the old extension.
- [Lazy contexts](../../tests/MustacheData__converts-lazy-object-contexts.phpt):
  ghosts and proxies initialize once through direct and nested conversion.
- [Initializer exceptions](../../tests/Mustache__preserves-lazy-initializer-exceptions.phpt):
  the original exception survives, including with invalid template source,
  and a later render succeeds.
- [Lambda ownership](../../tests/MustacheData__owns-lambda-values.phpt):
  retained objects and closures survive while needed and are released afterward.
- [Reinitialization](../../tests/MustacheData__rejects-reinitialization-during-render.phpt)
  and [unsupported values](../../tests/MustacheData__rejects-unsupported-values.phpt):
  populated data resists replacement, and reflection constructor bypass fails.
- [Property hooks](../../tests/MustacheData__object-property-hooks.phpt):
  initialized objects expose backing values without invoking getters.

The lazy-object and property-hook tests skip PHP versions before 8.4. The
sanitizer run therefore does not exercise lazy initialization. Traversal
invalidation and constructor reentry were not experimentally demonstrated.
Exceptions raised by a destructor during temporary-owner release were reviewed
statically, without a runtime experiment for that branch. Other platforms were
not tested. F7's Zend bailout cleanup concern remains separate: ordinary C++
scope cleanup does not establish bailout safety.

The independent code review found no in-scope defect. The independent test
review added the nested array/object capture check and PHP-exception cleanup
coverage above. Neither review demonstrated a production defect in this slice.

### Constructor review

The constructor review used the unchanged native implementation at `afaba55`.
Standalone checks passed on Linux x86-64 with PHP 8.3.33 and 8.4.24:

- Non-finite input emitted `E_WARNING`, left `toValue()` returning `false`, and
  caused `render()` to reject the uninitialized wrapper with `ValueError`.
- A completed retry with mixed array keys left the same instance uninitialized.
  A further retry whose warning handler threw preserved the exact exception
  object and still left no readable payload.
- A subsequent sequential retry with `['name' => 'Ada']` produced that exact
  value from `toValue()` and rendered `Ada`. A later replacement attempt warned
  and preserved the existing value.
- When a warning handler threw during ordinary `new`, the assignment target kept
  its previous value.
- Reflection reported the class as final; constructor bypass and cloning were
  rejected. On PHP 8.4, both lazy-wrapper factories were rejected before their
  initializers ran.

The six existing constructor, transactional-error, inheritance, reinitialization,
unsupported-value, and serialization PHPTs also passed on both versions. Fresh
full-suite runs passed 254 tests with 10 skips on PHP 8.3, and 260 tests with four
skips on PHP 8.4. All configured pre-commit checks, 14 local document links, and
the package-manifest check passed. These runs reused the matching native builds;
compilation, sanitizer checks, and other PHP versions were not repeated for this
documentation-only slice.

These are characterization checks, with no runtime implementation change or
red/green bug reproduction. Every constructor retry completed before the next
began. Neither overlapping initialization nor lifecycle manipulation inside an
input callback was exercised. The warning-handler timing conclusion comes from
source review. Zend bailout cleanup and other platforms remain unverified here.

## Separate finding from the cleanup fixture

A public `__destruct()` currently becomes a retained method lambda, contrary
to the API guide. A benign check with the previous PHP 8.4.24 extension found
the `__destruct` key in `MustacheData::toValue()`. The cleanup test therefore
uses property-only objects to avoid conflating intentional lambda retention
with temporary capture ownership. Correcting destructor exposure is newly
identified follow-up work, outside this F1 slice.
