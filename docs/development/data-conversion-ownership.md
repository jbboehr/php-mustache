# Data conversion ownership review

The F1 design review at revision `0e5503a` proposed retaining lazy-object
initialization with an explicit ownership contract. The implementation slice
based on `b506a28` now adds owned container captures. The original concern about
invalid native access has not been experimentally demonstrated. Constructor
publication and Zend bailout cleanup remain separate follow-ups.

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

`MustacheData::__construct()` rejects an existing native payload, converts into
a `unique_ptr`, and publishes only after conversion returns. The existing
reinitialization test covers an already initialized instance.

`MustacheData` is now final. Reflection-based constructor bypass is rejected,
and ordinary construction does not return the instance until the constructor
finishes. Those restrictions narrow the original publication concern.
However, a conversion failure under a nonthrowing warning handler can leave
an exposed, uninitialized instance, as documented in the
[PHP API guide](../php-api.md#errors). Finality alone does not settle that case.

This review has not established a supported caller that reenters conversion on
such an instance. Keep the publication question separate from traversal
ownership. An initialization-in-progress flag needs an applicability review
before it is added.

## Verification

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

## Separate finding from the cleanup fixture

A public `__destruct()` currently becomes a retained method lambda, contrary
to the API guide. A benign check with the previous PHP 8.4.24 extension found
the `__destruct` key in `MustacheData::toValue()`. The cleanup test therefore
uses property-only objects to avoid conflating intentional lambda retention
with temporary capture ownership. Correcting destructor exposure is newly
identified follow-up work, outside this F1 slice.
