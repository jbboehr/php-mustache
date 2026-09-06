# Zend bailout ownership review

This is the F7 design slice for php-mustache at `bd30d9e`, with libmustache
revision `fea4160d02238c7503d72e4f2705fda7b65edef5` from `flake.lock`. It follows
the [original finding](project-review-2026-09-04.md#f7-zend-fatal-error-bailouts-bypass-native-callback-cleanup)
and the [data ownership review](data-conversion-ownership.md).

F7 remains open. The source supports an ownership and state-restoration concern
when a Zend bailout crosses native frames, but this review has not demonstrated
an extension leak or persistent-worker failure. The recommendation is to design
the boundary between Zend and C++ before adding runtime handling. A catch around
the entire render operation would not preserve the cleanup it jumps past.

## Distinguish the termination mechanisms

Ordinary PHP exceptions return through the callback API with `EG(exception)`
set. The extension detects that state and throws `PhpInvalidParameterException`
to unwind its C++ owners. Its exception translator preserves the pending PHP
exception. This is the path exercised by the existing lambda, template getter,
and warning-handler exception tests. See [callback conversion](../../mustache_lambda.cpp)
and [exception translation](../../mustache_exceptions.cpp).

Ordinary `exit` also needs separate treatment. The normal exit paths in
[PHP 8.0.30](https://raw.githubusercontent.com/php/php-src/php-8.0.30/Zend/zend_vm_def.h),
[8.3.33](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend_vm_def.h),
and [8.4.24](https://raw.githubusercontent.com/php/php-src/php-8.4.24/Zend/zend_builtin_functions.c)
use `zend_throw_unwind_exit()`. PHP 8.4's
[implementation](https://raw.githubusercontent.com/php/php-src/php-8.4.24/Zend/zend_exceptions.c)
places an internal exit object in `EG(exception)`. That mechanism is not a
user-catchable exception or a direct `zend_bailout()` call. An ordinary exit
check alone would therefore not establish F7's bailout behavior. Failures in
output or cleanup while terminating still need their own analysis.

A Zend bailout uses a different route. The
[PHP 8.3.33 macros](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend.h)
install and restore a jump target. Its
[`_zend_bailout()` implementation](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend.c)
marks unclean shutdown, resets execution state, and performs a non-local jump.
The [fatal-error handler](https://raw.githubusercontent.com/php/php-src/php-8.3.33/main/main.c)
uses that route for errors it cannot recover from. A following `EG(exception)`
check or C++ `catch` is not a substitute for handling that boundary.

This also has a language-level constraint: C++17 does not define a
`setjmp`/`longjmp` pair that bypasses automatic objects whose non-trivial
destructors would run under exception unwinding. Moving the landing point to an
outer `zend_try` does not remove that constraint. See the
[C++17 draft's runtime support rules](https://timsong-cpp.github.io/cppwp/n4659/support.runtime).

## Owners and Zend boundaries

The relevant surface extends beyond explicit template lambdas:

| Boundary | Native ownership or cleanup obligation |
| --- | --- |
| [Lambda invocation and result coercion](../../mustache_lambda.cpp) | Result and argument zvals, section helper, and the enclosing renderer's data, templates, partials, and output. Result coercion can call `__toString()`. |
| [Template and partial source reads](../../mustache_mustache.cpp) | A getter can run while converted data or earlier compiled partials already exist. The temporary property value and copied source also need cleanup. |
| [Recursive object conversion](../../mustache_data.cpp) | Property handlers can run PHP while container captures, keys, active-path sets, and partially converted native data are live. |
| Native-to-PHP result construction | `RETVAL_STRINGL`, PHP array construction, and object initialization can allocate through Zend while C++ owners still exist. Callback-only coverage misses these boundaries. |
| [Temporary zval release](../../mustache_zval.hpp) and native lambda destruction | Releasing a final PHP reference can invoke object cleanup. That can happen during normal return or while another C++ exception is already unwinding. |
| [Nested helper rendering](../../mustache_lambda_helper.cpp) | Local AST and output storage coexist with the caller's render context and additional native render frames. |

Native lambda destruction releases its retained PHP values in
[ZendClosureLambda](../../mustache_zend_closure_lambda.cpp) and
[ClassMethodLambda](../../mustache_class_method_lambda.cpp). A cleanup design must
account for those releases as well as the explicit callback invocation.

libmustache contributes obligations beyond freeing heap storage. At the pinned
revision, [renderer scope guards](https://raw.githubusercontent.com/jbboehr/libmustache/fea4160d02238c7503d72e4f2705fda7b65edef5/src/renderer.cpp)
invalidate the callback's helper context, restore nested output state, and
decrement callback depth. Its
[render engine scopes](https://raw.githubusercontent.com/jbboehr/libmustache/fea4160d02238c7503d72e4f2705fda7b65edef5/src/render_engine.hpp)
also unregister active engines and reset render state. The
[helper context](https://raw.githubusercontent.com/jbboehr/libmustache/fea4160d02238c7503d72e4f2705fda7b65edef5/src/lambda.cpp)
uses automatic lock guards around nested rendering.

These are static reasons to require native unwinding or an equivalent explicit
lifecycle design. Reclaiming only the outer data and output allocations would
not establish that renderer state, helper validity, and synchronization are
restored. No runtime failure of those invariants was demonstrated here.

## What request shutdown already owns

Do not count every live PHP value as a persistent leak. PHP's
[request shutdown](https://raw.githubusercontent.com/php/php-src/php-8.3.33/main/main.c)
includes extension shutdown hooks and executor teardown. Its
[object-store cleanup](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend_objects_API.c)
can call registered `free_obj` handlers for surviving objects. Published native
payloads in `Mustache`, `MustacheData`, and `MustacheAST` have such handlers.
That is an existing cleanup route, although it does not prove cleanup completes
if a handler itself is interrupted.

Automatic native temporaries have a different owner. A local `std::string`,
container, or compiled handle is not registered with Zend merely because the
extension created it during a request. The extension's
[module definition](../../php_mustache.cpp) has no `RINIT` or `RSHUTDOWN` handler
that tracks those temporaries.

The sanitizer configuration's `USE_TRACKED_ALLOC=1` is also narrower than a
native ownership solution. PHP's
[allocator implementation](https://raw.githubusercontent.com/php/php-src/php-8.3.33/Zend/zend_alloc.c)
tracks allocations routed through its custom Zend heap. It does not interpose
on all C++ `new` or standard-library allocations. The settings in
[nix/derivation.nix](../../nix/derivation.nix) therefore do not close F7.

## Implementation direction

Evaluate narrow adapters around Zend operations that may abort, with a distinct
internal abort result propagated through ordinary C++ unwinding. The aim is to
prevent the jump from crossing extension or libmustache owners in the first
place, then preserve PHP's request-abort behavior after native cleanup. This is
a candidate architecture, not a validated patch or a new public exception API.

The first implementation slice should settle two linked questions: where the
adapters can establish a valid jump boundary, and how PHP references can be
released after an abort without starting another unsafe cleanup path. A callback
adapter alone would leave the coercion, allocation, and destruction rows above
uncovered. In particular:

- Restoring `EG(bailout)` must work for normal return, PHP failure, native
  exceptions, and nested extension calls. C++ exceptions must not escape through
  Zend's C frames.
- Abort propagation must preserve the original termination behavior. The
  existing catch-all translator must not turn an internal abort into a warning,
  a replacement exception, or a successful render.
- Cleanup during native unwinding must not throw a second C++ exception from a
  destructor. The current zval-owning destructors can call PHP, so they need an
  explicit cleanup contract before this approach is usable.
- Both render backends and nested helper calls must restore library state. A
  design limited to extension-owned allocations leaves dependency scope guards
  unaccounted for.

A request-shutdown registry could supplement that design, but registering only
the outer render objects would not cover local library objects or undo skipped
scope guards. Allocator substitution alone has the same state-restoration gap.
Neither is an adequate standalone fix for the reviewed implementation.

The next bounded task is a boundary and cleanup prototype covering callback
invocation, result conversion, and nested helper unwinding. Start with controlled
internal abort-status injection and ordinary exceptions to check cleanup and
propagation. Such checks would validate the prototype's C++ mechanics; they
would not establish actual Zend bailout safety. Production integration still
requires evidence for the remaining boundaries and supported runtimes.

## Verification and limits

Source review covered the extension at `bd30d9e`, the pinned libmustache files
linked above, PHP 8.3.33 bailout and shutdown implementation, and the normal exit
paths in PHP 8.0.30, 8.3.33, and 8.4.24. This was not a comprehensive PHP or
libmustache audit.

Fresh Linux x86-64 PHP 8.3.33 checks passed using the existing workspace module:

- [Lambda exception cleanup](../../tests/Mustache__render-lambda-exceptions-release-values.phpt).
- [Template getter and cleanup exceptions](../../tests/Mustache__template-source-exceptions-release-values.phpt).
- [Nested helper exception recovery](../../tests/MustacheLambdaHelper__nested-exception-contexts.phpt).
- [Warning-handler exceptions](../../tests/Mustache__render-warning-handler-exceptions.phpt).

These tests check observable reference release, exception preservation, helper
invalidation, and renderer recovery through ordinary exception unwinding. They
were not modified for this slice and do not exercise a Zend bailout.

The full PHP 8.3 suite also passed: 254 tests passed, 10 skipped, and no failures
or warnings. All configured pre-commit checks passed, including an explicit
Markdown check of the new document. The two changed documents passed 32 local
link checks, and the package manifest matched all 264 PHPT files. Runs used
`REPORT_EXIT_STATUS=1 NO_INTERACTION=1`, the matching PHP executable, and
`run-tests.php -n -d extension=MODULE`. Native builds, sanitizer checks, other
PHP runtimes, and other platforms were not repeated for this documentation slice.

No forced bailout, resource-exhaustion, or persistent-worker experiment was run.
Native leak volume, aborted-request state restoration, and cross-request effects
remain unverified. The standalone non-local-jump control from the original
report was not repeated: undefined C++ behavior cannot validate a recovery
strategy. No runtime implementation or public documentation changed.
