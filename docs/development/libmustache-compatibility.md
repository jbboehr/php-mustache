# libmustache compatibility

The extension currently pins libmustache
`c155e10070aa0d7bfdb22b5e2a4e0b8fc05c69f6` from `master` in
[flake.lock](../../flake.lock), which is the `v0.6.2` tag. Check this note when
changing that dependency or preparing a release.

## Build and deployment

The development line reports version 0.6.2 and SONAME `libmustache.so.6`.
The earlier transition from `e6b2de0` to `c43ad03` added lambda virtual methods
and changed renderer layout, requiring consumer rebuilds. A version-only
dependency on 0.6.0 cannot distinguish those incompatible snapshots.

The update from `c43ad03` to `efacbb5` adds non-virtual render-context
helpers. The PHP binding continues using its existing APIs. The historical ABI
break above does not describe this update.

The switch from `efacbb5` to `b4b60fe` is the `v0.6.0` tag and changes only
upstream release notes; the library implementation is unchanged.

The switch from `b4b60fe` (`v0.6.0`) to `c155e10` (`v0.6.2`) keeps ABI 6. It
adds packaging, Windows SDK, LTO, and YAML 1.1 typed scalars. The Nix package
builds without libyaml, so PHP data conversion is unaffected. The PHP binding
continues using its existing APIs.

Deploy the library revision used to verify the extension. The Nix build uses
the exact library store path. Source CI and the Unix PIE smoke script build both
components from the locked revision. The Windows and macOS binary jobs consume
the matching release SDKs pinned in `.github/libmustache-sdk.json`, verifying
their checksums before extraction. See [binary releases](binary-releases.md)
for the tested platform matrix and publication workflow.

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

Upstream also provides `LambdaRenderContext::renderResult()` and
`renderTemplate()` helpers that return literal results. The PHP helper keeps
its existing [rendering contract](../php-api.md#section-helpers): it tokenizes
with default delimiters and returns a string. PHP callbacks can wrap that
string in `MustacheLiteralResult` to preserve already-rendered text.

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
