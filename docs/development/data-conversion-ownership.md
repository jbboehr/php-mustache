# Data conversion ownership review

This is the F1 follow-up at revision `0e5503a`, reviewed on September 5, 2026.
The recommendation is to keep lazy-object initialization and give recursive
conversion an explicit ownership contract. The capture strategy below is a
proposal, not implemented behavior. F1's lifetime concern remains unconfirmed.

## Current behavior and evidence

[DataConverter](../../mustache_data.cpp) traverses borrowed array entries and
object properties. Recursive conversion can call an object's `get_properties`
handler while an outer traversal is still active. In PHP 8.4.24, the
[standard handler](https://raw.githubusercontent.com/php/php-src/php-8.4.24/Zend/zend_object_handlers.h)
can enter lazy initialization. Its
[property-table builder](https://raw.githubusercontent.com/php/php-src/php-8.4.24/Zend/zend_object_handlers.c)
also creates indirect entries pointing into declared property storage.

The converter now checks for a pending PHP exception immediately after
`get_properties`. Ordinary lazy ghosts and proxies, initializer exceptions,
and subsequent renderer reuse have regression coverage. The original report's
missing-exception-check observation is therefore resolved.

The remaining static concern is ownership during recursive traversal.
`ActivePathGuard` tracks array, object, and reference identities for cycle
detection but does not acquire a Zend reference. Retaining an object alone
would keep it alive without making its property values or table immutable.
Likewise, retaining an outer array does not independently capture values held
through PHP references. Adding one `ZvalGuard` at the root would not establish
a stable traversal contract for the whole conversion.

These observations justify reviewing ownership. They do not establish that a
particular application can invalidate an active traversal or cause an invalid
native access.

## Proposed capture contract

Capture each container's immediate entries before recursively converting its
children. Each captured entry must own its key and resolved value. Objects
remain the same PHP objects, and their properties are collected when conversion
reaches them. This is a per-container capture, not an atomic copy of the entire
object graph. A nested object's later initialization can still affect objects
whose properties have not yet been collected.

Implement this in the shared converter so `MustacheData`, rendering, and
`debugDataStructure()` follow the same rules:

1. Retain the resolved input owner before calling a property handler. Keep the
   object alive through both property collection and method-lambda creation.
2. Check for a pending exception immediately after the handler returns. Capture
   only the entries the current visibility and key rules would accept, before
   any recursive child conversion can run PHP code.
3. Copy indirect entries' resolved values into owned storage. Define reference
   capture explicitly so a copied reference wrapper is not mistaken for an
   independent value. Retain the original owners and identities needed by cycle
   detection, even if the traversal uses separate entry storage.
4. Charge entry and key-byte budgets before allocating their capture storage.
   Preserve node and depth limits during recursion. Review peak memory usage
   because captures coexist with the native result and ancestor captures.
5. Convert the owned entries, then release temporary ownership before reporting
   success. Releasing a final PHP reference can run a destructor. Preserve any
   pending PHP exception, and do not publish a successful result if cleanup
   raises one. C++ exception paths must release the same temporary ownership.

Keep the current array ordering, mixed-key rejection, property visibility,
backing-value behavior for property hooks, property-over-method precedence,
and lambda retention. Capturing values must not invoke userland clone hooks or
turn object properties into calls to getters. A different Zend property API
would need its own compatibility review.

The main compatibility decision is when values become fixed during conversion.
The proposed contract fixes immediate values when their container is captured.
It deliberately leaves later object-property collection observable. Review
that behavior and the extra allocation cost before implementing the capture.
Capturing all keys before recursion can also move key or limit errors ahead
of a child's exception. Tests must establish the intended error ordering.

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

Fresh full PHPT runs on Linux x86-64 used the existing matching extension builds
with unchanged native source:

| PHP | Passed | Skipped | Failed |
| --- | ---: | ---: | ---: |
| 8.3.33 | 253 | 7 | 0 |
| 8.4.24 | 256 | 4 | 0 |
| 8.5.9 | 256 | 4 | 0 |

Relevant passing coverage includes:

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

The lazy-object and property-hook tests skip PHP 8.3. These tests validate
existing behavior, not the proposed capture contract. Traversal invalidation
and constructor reentry were not experimentally demonstrated. No fresh native
build, sanitizer run, or other-platform verification was performed for this
documentation slice.

All configured pre-commit checks, explicit Markdown lint of this new file,
and checks of its eight local links and Markdown anchors passed.

The next implementation slice should establish owned per-container capture
with tests for its chosen semantics, reference identity, budgets, and cleanup.
F1 remains open until that work is verified. F7's Zend bailout cleanup concern
is separate: ordinary C++ scope cleanup does not establish bailout safety.
