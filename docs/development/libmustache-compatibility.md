# libmustache compatibility

The extension currently pins libmustache
`c43ad034850bab310d754bc3bba760cc31b08ef4` in
[flake.lock](../../flake.lock). Check this note when changing that dependency or
preparing a release.

## Build and deployment

This development snapshot reports version 0.6.0 and SONAME `libmustache.so.6`,
as did the preceding snapshot. Added lambda virtual methods and renderer layout
changes require consumer rebuilds. Those snapshots are not binary
interchangeable, and a version-only dependency on 0.6.0 cannot distinguish them.

Rebuild and deploy libmustache and php-mustache together. The Nix build uses the
exact library store path. Repository CI and PIE smoke scripts build both
components from the locked revision. Release packaging must preserve that
compatibility requirement.

## AST source ownership

Source-parsed ASTs retain their original source and parse-time tokenizer
settings. When preparing one as a partial, the extension validates its tree
and reparses that snapshot into an independent owned tree. A field-by-field
copy would lose libmustache's private section-source metadata.

Binary-loaded ASTs have no retained original source and use the validated
public-field clone path. Both paths keep the existing depth, node, and data-part
limits. Source AST partials incur extra source storage and reparsing work.

The [original section-body regression](../../tests/Mustache__render-preserves-original-section-body.phpt)
and [parser-settings regression](../../tests/Mustache__render-AST-partial-preserves-parser-settings.phpt)
cover this distinction.

## Callback results

PHP exposes [string interpretation and explicit results](../php-api.md#php-lambdas).
Ordinary callback strings default to template evaluation.
`MustacheLiteralResult` and `MustacheTemplateResult` override that setting for an
individual callback result.

The dedicated final classes own immutable text. Reusing mutable, subclassable
`MustacheTemplate` would make result recognition depend on its property or
string-conversion behavior. The [callback bridge](../../mustache_lambda.cpp)
recognizes the result class before string coercion and copies its text into an
owning native result while the PHP value remains retained.

Each operation captures string mode before PHP callbacks and passes it to the
compiled or archived renderer. AST input applies the captured mode to the shared
renderer immediately before rendering, preserving its existing overlap
rejection. Avoid temporarily changing and restoring instance settings around
callbacks or Fiber suspension.

If a release changes the default to literal mode, migration guidance must show
how to retain template interpretation with
`setLambdaStringMode(Mustache::LAMBDA_STRING_TEMPLATE)` or explicit template
results. Keep the lambda specification tests in template mode.

## Cache formats and migration

Experimental archive generation 3 preserves original section callback text,
including comments, delimiter spelling, and embedded NULs. Generations 1 and 2
are rejected. Regenerate those caches from template source after upgrading.

The optional archive format is separate from the public
`MustacheAST::toBinary()` format. Old public AST binaries remained readable
across this dependency update, subject to the existing format's source limits.
New serialization rejects section bodies whose original spelling or custom
delimiters cannot be preserved. Cache source for those templates, as described
in the [PHP API guide](../php-api.md#templates-and-partials).

[Section-body serialization](../../tests/MustacheAST__section-body-serialization.phpt),
[custom-delimiter serialization](../../tests/MustacheAST__rejects-lossy-custom-delimiter-serialization.phpt),
and [archive section source](../../tests/Mustache__archive-benchmark-section-source.phpt)
cover these contracts. Archive caching remains an optional benchmark experiment.
Promotion to a public API is a separate decision from dependency updates.

Release migration notes should also cover changed dotted-name lookup and
escaping of complete evaluated lambda interpolation results. Their behavior is
recorded by the [dotted-name regression](../../tests/Mustache__render-dotted-name-context-stack.phpt)
and [interpolation-escaping regression](../../tests/Mustache__render-escapes-complete-lambda-interpolation.phpt).
