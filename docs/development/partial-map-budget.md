# Partial-map budgets

`Mustache::setPartialLimits()` provides opt-in entry and text limits. Both
settings default to `null`, and zero enforces a zero allowance. The
[PHP API guide](../php-api.md#partial-map-limits) defines the public contract.

## Accounting

Each operation captures the instance settings before calling PHP and consumes
its own remaining allowance during partial preparation. A size check precedes
subtraction, so accumulation cannot overflow. Nested calls and suspended Fibers
do not share counters. The optional archive serializer uses the same preparation
helpers and captures its own limits.

[Partial preparation](../../mustache_operations.cpp) charges every supplied entry
and its name, including unused entries and repeated values under different names.

| Representation | Text charged |
| --- | --- |
| Source string | Source bytes, before the owned native copy and compilation. |
| `MustacheTemplate` | The source returned by its property read, before the native copy and compilation. |
| Source-parsed `MustacheAST` | Retained original source, before reparsing with its saved tokenizer settings. |
| Binary-loaded `MustacheAST` | String fields and nested partial names copied by the clone path, charged during validation. |

AST preparation retains its per-AST depth, node, and data-part limits.
Repeated AST entries are prepared separately, so deduplicating accounting by
PHP object identity would undercount their cost.

The text budget measures input and prepared text. Source compilation can add
nodes and string copies outside this count. Serialized AST byte length also
does not describe all clone allocations, and serializing just to size a budget
would allocate before the check. Root templates, data conversion, and rendering
have separate limits.

## Source reads and error order

Preparation visits entries in order, including unused ones. It reads each
wrapper once. Invalid earlier source stops preparation before a later wrapper
getter runs.

Preserve this order when changing accounting. Resolving every wrapper in a
preflight pass could run PHP code that would otherwise never execute. Reading
wrappers a second time could observe different source.

## Deferred defaults and complete memory accounting

No aggregate limit is enabled by default. The existing benchmark fixtures have
at most three partials and do not establish thresholds for applications loading
large catalogs. No production workload distribution or process-memory target
was available when this decision was reviewed.

Revisit defaults when a supported deployment provides partial counts, total
name and source bytes, AST usage, and an acceptable memory budget. Include many
small entries as well as a few large templates. Define accounting units,
failure behavior, and configuration scope before adding enforcement.

A complete node budget needs a libmustache contract for accounting across opaque
compiled handles. Reusing one per-template limit for every partial does not
create a shared allowance. The opt-in entry and text limits make no claim to
bound total process memory, and the review did not establish a
resource-exhaustion defect.

## Regression coverage

The tests cover
[disabled, zero, and inclusive limits](../../tests/Mustache__partial-limits.phpt),
[representation-specific accounting](../../tests/Mustache__partial-limits-representations.phpt),
[getter order and nested calls](../../tests/Mustache__partial-limits-callbacks.phpt),
[Fiber suspension](../../tests/Mustache__partial-limits-fibers.phpt),
and [archive preparation](../../tests/Mustache__partial-limits-archive.phpt).
They also check rejected configuration, per-call resets, and configuration
changes during callbacks using small inputs and caller-selected limits.
The Fiber test requires PHP 8.1 or later, and the archive test requires the
optional benchmark bridge.
