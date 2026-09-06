# Zend bailout adapter contract

This F7 slice follows the [ownership review](zend-bailout-ownership.md) and
its [standalone prototype](zend-bailout-ownership.md#boundary-prototype).
It reviews php-mustache at `540814c` against the PHP 8.3.33 sources linked below.
It defines requirements for a real adapter; no adapter is installed by this
change. The [feasibility decision](#integration-gate-and-next-work) at `587ee37`
now defers production integration. F7 remains unresolved.

The decision is to retain narrow Zend boundaries as a candidate, but **not to
integrate the prototype's cleanup callback with `zval_ptr_dtor()` yet**. The
prototype establishes native propagation with a simulated release operation.
Real PHP-reference release needs an ownership contract for both untouched
values and values whose destruction was interrupted.

## A caught bailout is a terminal outcome

PHP's [`_zend_bailout()`](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend.c)
protects GC, marks unclean shutdown, clears compiler state and
`EG(current_execute_data)`, then jumps. The
[`zend_try` macros](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend.h)
restore the previous bailout address. They do not reverse those other changes.
Consequently, restoring that address is not evidence that normal PHP execution
or arbitrary PHP cleanup can resume. The adapter must propagate termination
after its native callers have unwound; it must not restore selected executor
fields and continue rendering.

The real boundary needs to distinguish these outcomes:

| Outcome | Required caller behavior |
| --- | --- |
| Zend call returned, no PHP exception | Interpret the operation's return code and value using its existing API contract. |
| Zend call returned with `EG(exception)` | Preserve the PHP exception and use ordinary native unwinding. |
| Zend bailout intercepted | Mark the affected invocation aborted; do not consume or publish the operation's result. Propagate through native cleanup. |

The first row does not imply success. Conversely,
[`zend_call_function()`](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend_execute_API.c)
can return `SUCCESS` with a pending PHP exception. Its normal return paths also
perform call-frame cleanup and restore execution state; a non-local jump can
bypass those steps. An adapter cannot infer that its out-parameters are usable
merely because the call was entered. This preserves the distinction already
made by [lambda result conversion](../../mustache_lambda.cpp).

## PHP release is not a transaction

[`i_zval_ptr_dtor()`](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend_variables.h)
decrements a reference before dispatching destruction. Object destruction can
call a PHP destructor, and the
[standard object cleanup](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend_objects.c)
also releases property values. These operations can recursively enter other
cleanup. A release that does not return therefore cannot be treated as though
it never started.

An ownership design must distinguish at least the following states. These are
review requirements, not new runtime types:

| State | What is known | What the design must provide |
| --- | --- | --- |
| Held | This owner retains a PHP reference; release has not started. | An identified owner and release route, including during abort propagation. |
| Releasing | Release has been entered and may already have changed references or run user code. | Ownership metadata that remains valid across the boundary; no competing release of the same owned reference. |
| Released | Release returned normally, possibly with a PHP exception pending. | Retire the owner's slot exactly once and preserve any exception. |
| Interrupted | Release did not return. Its internal progress is unknown to this owner. | No retry or payload inspection based only on the old zval; a separate, justified shutdown disposition. |

Clearing a local zval before calling its destructor could prevent that local
owner from retrying. It does not establish what owns the unfinished operation,
reclaim its remaining resources, or authorize another release. Likewise,
skipping every PHP release after an abort would leave ownership obligations
unaccounted for. Neither change alone closes F7.

Shutdown is not a general retry mechanism. In PHP's
[object store](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend_objects_API.c),
the destructor and free-handler flags are set before their handlers run.
Normal object deletion also invalidates the object's store entry before calling
`free_obj`. Shutdown storage cleanup considers valid entries and skips handlers
already marked as called. This is evidence against assuming it will replay an
interrupted handler, not evidence of a reproduced extension leak.

The immediate extension owners to account for are
[`ZvalGuard`](../../mustache_zval.hpp),
[`ZvalArguments`](../../mustache_lambda.cpp), the conversion captures in
[`mustache_data.cpp`](../../mustache_data.cpp), and the retained values in
[`ZendClosureLambda`](../../mustache_zend_closure_lambda.cpp) and
[`ClassMethodLambda`](../../mustache_class_method_lambda.cpp).
Their current destructors release PHP values directly. For example,
`ClassMethodLambda` releases its object and then its function name; interruption
of the first release leaves the second statement unexecuted. A single boolean
saying that the native lambda was destroyed would not describe both obligations.

## Boundary placement and nested calls

Use operation-specific C-compatible boundaries as the design target. A boundary
must restore its saved jump target before returning, including on failure.
Do not put a render-wide `zend_try` around C++ owners: that retains the
[C++ non-local-jump constraint](zend-bailout-ownership.md#distinguish-the-termination-mechanisms).
Compiling the adapter as C would help enforce its local constraints, but would
not by itself make its entire call graph safe.
State read after the jump must also obey `setjmp`'s rules for automatic variables
changed since the jump target was established; caller-owned outcome storage
must remain alive throughout the operation.

A callback can call PHP, which can enter php-mustache again. Each nested native
invocation must unwind its own owners before propagating termination back to
the enclosing Zend boundary. An internal C++ abort marker must never escape an
extension entry point through Zend's C frames. This applies to object handlers
and cleanup entry points as well as public render methods.

Abort status must take precedence over ordinary error translation. The current
[`mustache_exception_handler()`](../../mustache_exceptions.cpp) can allocate PHP
exceptions or report fatal errors from inside C++ catch clauses. Integration
must keep an abort out of that translation path and end the catch scope,
including ownership of any saved C++ exception, before forwarding termination.
Cleanup during unwinding must not throw a second C++ exception. Native owners
whose destruction can release PHP values need the separate release contract
above before their destructors can satisfy that requirement.

The same rules apply to property access, coercion, PHP allocation, and result
construction, as listed in the
[boundary inventory](zend-bailout-ownership.md#owners-and-zend-boundaries).
Output publication must wait for those operations and final cleanup to finish.
The prototype's native output check does not cover allocation of the PHP return
value. Neither a callback-only adapter nor an outer allocation registry covers
all of these obligations.

## Request state needs a lifecycle decision

Keep abort state sticky while the affected invocation and its callers unwind;
nested rendering must not clear it. However, the prototype's `Request` object
is not yet a prescription for extension globals.

PHP 8.3.33's [Fiber implementation](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend_fibers.c)
saves and restores `EG(bailout)` with execution state and forwards bailout
between Fiber contexts. Any extension-side stack of active adapters must respect
those context lifetimes. A process-global saved jump address or a stack that
assumes all suspended renders return in nesting order is insufficient.

PHP's [request shutdown](https://raw.githubusercontent.com/php/php-src/php-8.3.33/main/main.c)
runs registered shutdown functions and object destructors before extension
`RSHUTDOWN` hooks. Such callbacks can invoke extensions. Keeping a request-wide
abort flag until `RSHUTDOWN` would therefore also affect rendering attempted by
shutdown callbacks. The real implementation must explicitly choose and validate
that behavior; silently copying the prototype's request-wide rejection rule
would add a compatibility decision the prototype has not tested.

## Integration gate and next work

**Decision at `587ee37`: defer F7's runtime implementation.** A request-owned
reference store has not established the missing interrupted-release contract.
The options below either change ordinary object lifetimes or require a broader
ownership redesign before they can satisfy this adapter contract. Retain the
existing implementation and its ordinary exception tests. This finishes the
current feasibility slice; further F7 work depends on the reopening conditions
below.

The scope is cleanup of php-mustache and libmustache owners, including PHP
references they retain. F7 does not require repairing every other extension's
cleanup. Nor does this decision establish that all real bailout adapters are
impossible: it rejects integrating the candidates reviewed here without the
missing ownership evidence. The extension's bailout effects remain unmeasured.

### Reference-store options

| Option | What it provides | Why it is not being implemented now |
| --- | --- | --- |
| Keep strong references in a request store until shutdown | Values outlive native stack unwinding. | Keeping those references after their normal owners die changes object lifetimes and delays collection of cycles. Shutdown release can still enter interrupted cleanup. |
| Release store entries when their normal owner dies | Can preserve ordinary release timing instead of retaining everything until shutdown. | It reaches the same effectful PHP destruction boundary. Moving that call into a store does not establish the disposition of an interrupted release. |
| Make a PHP owner hold the values, with native code borrowing them | Could separate native destruction from PHP release while preserving the object graph. | This is a possible redesign, not a drop-in registry. It needs ownership transfer, GC reporting, partial-construction handling, and coverage of temporary and published native owners. |

The third option is the most useful direction if F7 is reopened. Currently,
[`MustacheData_obj_get_gc()`](../../mustache_data.cpp) describes references held
by native lambdas. PHP's
[`zend_get_gc_buffer_add_zval()`](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend_gc.h)
copies value descriptors without acquiring references, and the
[collector](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend_gc.c)
uses the reported edges during cycle collection. Reporting a reference to GC
does not transfer its ownership or arrange abort cleanup. Moving ownership to
a PHP store would require the reported graph to match the new owners, without
omitting or double-counting their references.

Native deletion is not yet independent of that graph.
[`MustacheData_obj_free()`](../../mustache_data.cpp) deletes the native data,
whose lambda destructors release PHP values. A store would have to separate
those actions throughout the owned data, rather than only move the outer
`delete`. PHP's
[object-store teardown](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend_objects_API.c)
still provides no replay of a free handler already marked as called. Request
heap reclamation alone therefore does not establish completion of the extension's
native cleanup. This conclusion comes from source review, not an interrupted
handler experiment.

### Ordinary retention control

This PHP example compares the existing collectable native-lambda cycle with
the same cycle kept reachable by an extra strong reference. It models only the
first option's decision to retain references after their ordinary owners become
unreachable; it is not an implementation of a native store or a bailout test.

```php
<?php
function closureCycle(): array
{
    $holder = new stdClass();
    $closure = static function () use ($holder) { return 'ok'; };
    $data = new MustacheData(['value' => $closure]);
    $holder->data = $data;
    return [$closure, WeakReference::create($data)];
}

foreach ([false, true] as $retain) {
    [$closure, $weak] = closureCycle();
    $requestReferences = $retain ? [$closure] : [];
    unset($closure);
    gc_collect_cycles();
    echo $retain ? "retained\n" : "unrooted\n";
    var_dump($weak->get() === null);
    $requestReferences = [];
    gc_collect_cycles();
    var_dump($weak->get() === null);
}
```

PHP 8.3.33 with the unchanged workspace extension produced:

```text
unrooted
bool(true)
bool(true)
retained
bool(false)
bool(true)
```

The extra root keeps the cycle alive; clearing it allows collection. The
unrooted case also guards against attributing an existing collection failure
to the proposed store. This demonstrates the cost of deferred strong-reference
release, not a leak in the current extension or a defect in every possible
reference store. Weak references could observe such owners but would not keep
the PHP values alive for native borrowers.

### Conditions for reopening F7

Reopen runtime work when a concrete ownership proposal can account for all of
the following, with supporting source or runtime evidence:

- Native teardown can finish without releasing PHP values from destructors
  still needed for unwinding. Ownership metadata survives partial acquisition
  and the interrupted operation without depending on skipped native frames.
- Each retained PHP reference has an identified owner and GC edge throughout
  transfer, normal release, and interrupted release. The design preserves
  ordinary collection behavior or explicitly justifies a compatibility change.
- Temporary and published owners, nested calls, suspended Fibers, and shutdown
  callbacks have defined lifetimes. The design covers the complete
  [boundary inventory](zend-bailout-ownership.md#owners-and-zend-boundaries).

Then choose a bounded implementation and validation slice for that proposal.
The existing prototype and ordinary exception tests remain useful controls;
neither substitutes for evidence about the actual Zend boundary. Until then,
leave F7 deferred and move to the other planned work.

### Feasibility verification

Source review at `587ee37` covered the extension's native lambda owners,
`MustacheData` GC and free handlers, and the linked PHP 8.3.33 GC and object-store
implementations. The feasibility decision is an engineering judgment based on
those ownership requirements and the limited experiment above.

Fresh Linux x86-64 PHP 8.3.33 checks used the unchanged workspace module:

- The PHP block above was extracted from this document and executed. Its output
  matched the displayed result exactly, with exit status zero and no stderr.
- Five existing PHPTs passed: [lambda ownership](../../tests/MustacheData__owns-lambda-values.phpt),
  [capture release](../../tests/MustacheData__releases-captured-values.phpt),
  [lambda exception cleanup](../../tests/Mustache__render-lambda-exceptions-release-values.phpt),
  [source Fiber interleaving](../../tests/Mustache__render-source-fiber-contexts.phpt),
  and [AST Fiber overlap and recovery](../../tests/Mustache__render-AST-fiber-overlap-and-recovery.phpt).
- The full suite passed: 254 passed, 10 skipped, no failures or warnings, using
  the PHP runner settings recorded below.
- All configured pre-commit checks, Markdown lint, 84 local links across the
  three changed documents, and the manifest check passed. All 264 PHPTs remain
  listed and the maintainer documents remain excluded from the package.

No production implementation, new PHPT, or reference-store prototype was added.
The extension and C++ boundary prototype were not rebuilt. Interrupted release,
actual Zend bailout handling, cross-request effects, sanitizers, other PHP
versions, and other platforms were not tested. The Fiber checks establish
existing ordinary behavior, not the proposed store's lifecycle.

## Verification and limits

The following results belong to the preceding adapter-contract slice, before
the feasibility decision above.

Source review covered the linked PHP 8.3.33 bailout macros, executor calls,
reference release, object store, Fiber state, and request shutdown, together
with the extension owners and translator above. The ownership and propagation
requirements are design inferences from those sources. They are not results of
executing an interrupted Zend operation.

Fresh Linux x86-64 verification passed:

- The standalone prototype's 22 rendering cases and terminal-state checks, using
  `nix develop --command bash docs/development/prototypes/bailout-boundary/run.sh`.
- All four ordinary PHP exception controls listed in the
  [ownership review](zend-bailout-ownership.md#verification-and-limits).
- The full PHP 8.3.33 suite: 254 passed, 10 skipped, no failures or warnings,
  using the unchanged workspace module. Runs used `REPORT_EXIT_STATUS=1`,
  `NO_INTERACTION=1`, the matching `TEST_PHP_EXECUTABLE`, and
  `run-tests.php -n -d extension=modules/mustache.so -j4 tests`.
- All configured pre-commit checks and an explicit Markdown check covering the
  new document, 73 local links across the three changed documents, and package
  validation confirming all 264 PHPTs are listed and maintainer documents remain
  excluded.

The prototype was recompiled; the extension was not rebuilt. No actual Zend
adapter, non-local jump, interrupted PHP release, request-abort recovery,
sanitizer run, other PHP version, or other platform was tested. The source review
of Fiber and shutdown behavior was not a runtime test of the proposed lifecycle.
No production code, public API, or release manifest changed.
