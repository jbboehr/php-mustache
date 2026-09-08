# Upgrading to 0.10.0

This guide covers application changes when upgrading from php-mustache 0.9.x.
The [PHP API guide](php-api.md) describes the complete current contract.

## Runtime and build requirements

Use PHP 8.0 or newer, libmustache 0.6.0 or newer, and a C++17 compiler.
Rebuild the extension against the library you deploy. PHP 7 and libmustache
0.5 builds are no longer supported.

PIE installation is available alongside PECL and source builds. See the
[installation instructions](../README.md#installation).

## Handle rendering errors as exceptions

Successful `Mustache::render()` calls return strings, including `''` for valid
empty output. Invalid inputs no longer produce warning-only failures or partial
results. Replace checks for `false` or `null` with exception handling:

```php
<?php
$mustache = new Mustache();
try {
    echo $mustache->render('{{name}}', [0 => 'Ada', 'name' => 'Lin']);
} catch (TypeError | ValueError | MustacheException $error) {
    echo "Template could not be rendered\n";
}
// Template could not be rendered
```

`TypeError` identifies unsupported argument types. `ValueError` covers invalid
data and uninitialized wrappers. `MustacheParserException` extends
`MustacheException` and identifies invalid template source. Exceptions thrown
by application callbacks propagate unchanged.

`parse()` always returns a `MustacheAST` on success. Passing an existing AST
returns that same object. An empty string is valid source, but an uninitialized
`MustacheTemplate` or `MustacheAST` is rejected.

`MustacheData` retains its warning-based constructor behavior. A nonthrowing
warning handler can leave it uninitialized, and rendering that object throws
`ValueError`. See the [error contract](php-api.md#errors).

## Check scalar types and rendered text

Converted values retain PHP null, boolean, integer, and finite floating-point
types. `MustacheData::toValue()` returns those types instead of their old string
representations. Code comparing its output with strings must account for this.

Boolean `true` now renders as `true`, where 0.9.x rendered `1`. Null and
`false` still render as empty text. Numeric zero remains truthy in a section.
Floating-point output uses libmustache's formatting instead of PHP's
`precision` INI setting. Format values explicitly when the exact text matters:

```php
<?php
$mustache = new Mustache();
echo $mustache->render('{{enabled}}|{{amount}}', [
    'enabled' => true ? '1' : '',
    'amount' => number_format(1234.5, 2, '.', ''),
]), "\n";
// 1|1234.50
```

String keys and values now preserve embedded NUL bytes. Numeric-key arrays
remain sequences in insertion order, with consecutive indices when converted
back to PHP. Mixed integer/string keys, resources, non-finite floats, recursive
data, and values exceeding conversion limits are rejected.

## Update wrapper usage and subclasses

`MustacheData` and `MustacheLambdaHelper` are final. Replace `MustacheData`
subclasses with composition:

```php
<?php
final class ViewData
{
    public MustacheData $data;

    public function __construct(array $input)
    {
        $this->data = new MustacheData($input);
    }
}

$view = new ViewData(['name' => 'Ada']);
echo (new Mustache())->render('{{name}}', $view->data), "\n";
// Ada
```

Pass `MustacheData` as the entire data argument. To replace its contents, create
a new instance. Reinitializing an existing instance is rejected. The class
cannot be cloned or serialized, and reflection cannot bypass its constructor.

The extension supplies section helpers to callbacks. Use a helper only during
that callback. A retained helper throws `MustacheException` after the callback
returns. Do not instantiate, subclass, or persist helpers.

`Mustache` cannot be serialized. Persist its configuration and construct a new
instance instead. `Mustache`, `MustacheTemplate`, and `MustacheAST` remain
subclassable. Update method overrides to match the [PHP stub](../mustache.stub.php),
including parameter and return types.

Public object properties take precedence over methods with the same name.
Constructors and destructors are excluded from method lambdas. Data conversion
captures each container's immediate values before converting its children.
Properties already captured stay fixed, while method callbacks retain the
original object and can observe later changes.

## Review templates and callbacks

Dotted names are resolved through nested contexts. Replace literal dotted keys
such as `['profile.name' => 'Ada']` with nested data such as
`['profile' => ['name' => 'Ada']]`.

Standalone-tag whitespace and partial indentation follow Mustache's rules.
Check whitespace-sensitive output. Section callbacks receive the original
source spelling, comments, and whitespace rather than reconstructed body text.

Ordinary callback strings still default to template evaluation. Literal mode
and explicit result objects are opt-in. To preserve text already rendered by a
section helper, return `MustacheLiteralResult`:

```php
<?php
$mustache = new Mustache();
echo $mustache->render('{{#keep}}{{name}}{{/keep}}', [
    'name' => '{{other}}',
    'other' => 'Ada',
    'keep' => function (string $text, MustacheLambdaHelper $helper): MustacheLiteralResult {
        return new MustacheLiteralResult($helper->render($text));
    },
]), "\n";
// {{other}}
```

Escaped interpolation now escapes the complete evaluated lambda result,
including its literal fragments and partial output. Existing callbacks that
return pre-escaped HTML can therefore produce additional escaping. Prefer
unescaped text and let the template control escaping. Triple-brace or ampersand
tags emit unescaped output and should be used only for content intended to be
inserted as HTML. Literal results still obey the outer tag's escaping rules.

The PHP section helper continues to parse its input using `{{` and `}}`,
even when the surrounding section uses custom delimiters. See
[callback results and helpers](php-api.md#php-lambdas).

## Rebuild template caches

Invalidate or version cached templates when upgrading libmustache. Retain
template source so a cache entry can be regenerated.

Use `MustacheAST::toBinary()` and `MustacheAST::fromBinary()` for explicit binary
persistence. Casting an AST to a string still exports binary data, but is
deprecated in the tooling stub. Diagnostic arrays from `tokenize()`,
`debugDataStructure()`, and `toArray()` are not stable cache formats.

Binary export and PHP serialization can throw `MustacheException` when a
section's custom delimiters or original body text cannot be preserved.
Those ASTs still render. Cache their source instead of normalizing the body or
discarding its metadata. Existing serialized AST payloads remain supported
subject to validation, size limits, and the linked library's format support.
See the [cache example](../examples.md#persisting-parsed-templates).

The optional archived-template benchmark format remains experimental and
separate from the public AST binary format. It is not a new supported cache API.

## Check INI settings and input limits

The `mustache.default_escape`, `mustache.default_start`, and
`mustache.default_stop` INI settings now take effect correctly. Review deployed
settings that previously appeared to have no effect. Delimiters must be nonempty.

Conversion, parsing, rendering, and binary persistence enforce finite limits.
Applications that relied on deeper or larger inputs should check the
[documented limits](php-api.md#defaults-and-limits). Aggregate partial-map
limits remain disabled by default and can be enabled with `setPartialLimits()`.
