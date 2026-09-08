# Data conversion ownership

[DataConverter](../../mustache_data.cpp) is shared by `MustacheData`, rendering,
and `debugDataStructure()`. It owns container entries across PHP property
collection and recursive conversion. Zend bailout cleanup remains a
[separate unresolved concern](zend-bailout-ownership.md).

## Capture contract

Each container's immediate entries are captured before converting its children.
Captured entries own their keys and resolved values independently of the
original buckets and indirect property slots. Objects keep their identity, and
their properties are collected when conversion reaches them. This captures one
container at a time. Later initialization can still affect objects whose
properties have not yet been collected.

The converter preserves these ownership rules:

1. The resolved input owner survives property collection and method-lambda
   creation. A pending PHP exception is checked immediately after
   `get_properties` returns.
2. `CapturedValue` copies reference referents and retains their original reference
   wrappers separately. `ActivePathGuard` tracks original array, object, and
   reference identities along the current recursion path, so sibling aliases
   remain valid.
3. Entry and key-byte budgets are charged before allocating capture storage.
   Node and depth limits remain enforced during recursive conversion.
4. Temporary captures and the root owner are released before success or
   publication. C++ exception paths release the same owners. A PHP exception
   raised during cleanup stops conversion and takes precedence over translating
   a native validation error.

Array ordering, mixed-key rejection, property visibility, backing values for
property hooks, property-over-method precedence, and lambda retention are
preserved. Capturing entries does not invoke userland clone hooks or property
getters, though Zend property collection can initialize lazy objects.

Key validation and capture-budget errors occur before child conversion.
Captures coexist with ancestor captures and the native result. Entry and
key-byte limits bound this additional capture storage, but do not bound total
process memory. Throughput and peak resident-memory costs have not been
benchmarked.

## Constructor publication

`MustacheData::__construct()` rejects an existing native payload, converts into
a `unique_ptr`, and publishes only after conversion and temporary-owner cleanup
succeed.

| Entry path | Behavior |
| --- | --- |
| Ordinary `new MustacheData($input)` | Input callbacks are not given the wrapper. The expression returns it after construction finishes. |
| Repeated constructor call on initialized data | The payload check rejects replacement before converting new input. |
| Failed conversion with a nonthrowing warning handler | The expression can return an uninitialized wrapper. `toValue()` returns `false`, and rendering rejects it with `ValueError`. |
| Sequential retry on an uninitialized wrapper | A later direct constructor call can initialize it. Failed retries leave it uninitialized. |
| Subclassing, reflection constructor bypass, cloning, or serialization | The wrapper's finality and allocation, clone, and serialization restrictions reject these paths. |
| Lazy `MustacheData` wrapper on PHP 8.4 | PHP rejects both lazy ghosts and proxies for this internal class. Lazy input objects remain supported. |

Native validation warnings reach the
[exception handler](../../mustache_exceptions.cpp) after conversion has unwound
out of the constructor's `try` block. A warning handler cannot resume that
failed conversion and cause it to publish a result. PHP exceptions from input
callbacks also stop publication. See the [public error contract](../php-api.md#errors).

The constructor has no initialization-in-progress flag. The documented
construction and rendering flows do not require overlapping direct constructor
calls on one uninitialized instance, and their safety has not been established.
The review retained the existing state model because it found no affected
supported caller. Revisit this decision if a factory exposes wrappers before
initialization completes or an application requires overlapping initialization.

## Destructor methods

Object conversion excludes `__destruct` from method lambdas by a case-insensitive
name comparison. PHP 8.3 has no `ZEND_ACC_DTOR` flag. Pointer identity also misses
inherited internal methods that PHP copies into a child's method table while
retaining the parent's destructor pointer.

Constructor, visibility, and static-method filtering keep their existing rules.
Ordinary public methods and other public magic methods still become lambdas.
A public property named `__destruct` remains data. An object with no eligible
method lambdas is not retained solely for its destructor, while objects with
ordinary method lambdas stay alive for those callbacks.

## Regression coverage

The ownership contract is covered by tests for
[captured values](../../tests/MustacheData__captures-container-values.phpt),
[key validation before child conversion](../../tests/MustacheData__validates-keys-before-child-conversion.phpt),
[capture cleanup](../../tests/MustacheData__releases-captured-values.phpt),
[PHP-exception cleanup](../../tests/MustacheData__releases-captures-after-php-exception.phpt),
[lazy contexts](../../tests/MustacheData__converts-lazy-object-contexts.phpt),
[initializer exceptions](../../tests/Mustache__preserves-lazy-initializer-exceptions.phpt),
and [property hooks](../../tests/MustacheData__object-property-hooks.phpt).
Lazy-object and property-hook cases require PHP 8.4 or later.

[Reinitialization rejection](../../tests/MustacheData__rejects-reinitialization-during-render.phpt)
and [transactional errors](../../tests/MustacheData__conversion-errors-are-transactional.phpt)
cover publication.
[Property copies and retained method state](../../tests/MustacheData__copies-properties-and-retains-method-state.phpt)
cover the distinction between captured data and live method callbacks.

Destructor filtering has regressions for
[userland methods](../../tests/MustacheData__omits-destructor-methods.phpt),
[inherited internal methods](../../tests/MustacheData__omits-inherited-internal-destructor.phpt),
and [ordinary method lifecycle](../../tests/Mustache__render-object-method-lifecycle.phpt).
Ordinary exception and release tests do not establish cleanup across Zend
bailouts.
