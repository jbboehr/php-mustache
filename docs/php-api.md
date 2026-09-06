# PHP API guide

Use the [official Mustache manual](https://mustache.github.io/mustache.5.html)
for template syntax, sections, escaping, and partials. This guide covers how
the PHP extension accepts data and calls PHP code. The
[PHP stub](../mustache.stub.php) lists public methods and their types.

## Templates and partials

`Mustache::render($template, $data, $partials)` accepts a source string, an
initialized `MustacheTemplate`, or an initialized `MustacheAST`. Partials are
an optional array with string keys and values of those same template types.
All supplied partials are validated, including ones the template never uses.

Use `Mustache::parse()` to create an AST from source. Passing an existing AST
to `parse()` returns that same object. `new MustacheAST($binary)` and
`MustacheAST::fromBinary($binary)` read binary AST data, not template source.
See the [parsed-template cache example](../examples.md#persisting-parsed-templates)
for storing binary templates and invalidating them after libmustache upgrades.

An empty string or `new MustacheTemplate('')` is a valid empty template.
`new MustacheTemplate()` and `new MustacheTemplate(null)` leave their wrappers
uninitialized. Passing these to `parse()` or `render()` throws `ValueError`.
`new MustacheAST()` and `new MustacheAST(null)` throw `MustacheException`
because empty input is not a valid binary AST. Use `parse('')` for an empty AST.
An AST whose constructor was bypassed is uninitialized and throws `ValueError`
from `parse()`, `render()`, `toArray()`, and `toBinary()`.

## PHP data

Data can contain `null`, booleans, integers, finite floats, strings, arrays,
and objects. Strings preserve embedded NUL bytes. Resources, non-finite
floats (`INF`, `-INF`, and `NAN`), and cycles encountered while traversing
arrays or object properties are rejected.

PHP arrays become one of two Mustache data shapes:

| PHP keys | Mustache data |
| --- | --- |
| All integer keys | A sequence in PHP iteration order. Original indices are discarded. |
| All string keys | A map whose keys are available to template lookups. |
| Both integer and string keys | Invalid data. |

An empty array becomes an empty sequence. PHP can convert numeric string keys
to integers before the extension receives them.

```php
<?php
$mustache = new Mustache();
echo $mustache->render('{{#items}}[{{name}}]{{/items}}', [
    'items' => [['name' => 'Ada'], ['name' => 'Lin']],
]), "\n";
```

Output:

```text
[Ada][Lin]
```

### Objects

Ordinary objects expose initialized public instance properties, including
inherited and dynamic properties. Private, protected, static, and
uninitialized typed properties are omitted. Public instance methods become
lambdas under their declared names, including capitalization. Constructors,
destructors, and static methods are omitted. A public property takes
precedence over a method with the same name.

Missing properties are not discovered through `__get()`, `__isset()`, or
`__debugInfo()`. `ArrayObject` offsets are not treated as properties, even
with `ARRAY_AS_PROPS`. Pass `getArrayCopy()` to expose those offsets as data.
On PHP 8.4 and newer, conversion can initialize a lazy object, and an
exception from its initializer propagates to the caller.

Closures and objects with `__invoke()` become lambdas themselves. Other PHP
callable forms, such as function-name strings and callable arrays, remain
ordinary string or array data.

### Reusing converted data

`MustacheData` converts data once for use in multiple renders. Pass it as the
entire data argument, not as a nested array or object value.

```php
<?php
$input = ['name' => 'Ada'];
$data = new MustacheData($input);
$input['name'] = 'Lin';

echo (new Mustache())->render('{{name}}', $data), "\n";
```

Output:

```text
Ada
```

Scalar and container values are copied. Lambdas retain their PHP closure or
object, so they can still read changing PHP state. For example, an object's
copied property and a retained method that reads that property can produce
different values after the original object changes.

`MustacheData::toValue()` reconstructs scalars and arrays. Object maps become
associative arrays, and sequences have consecutive integer indices. Each
lambda becomes `null` and emits an `E_WARNING`, so this method does not
provide a lossless round trip for callable data. `MustacheData` cannot be
subclassed, cloned, or serialized with PHP's `serialize()`.

## PHP lambdas

Interpolation calls a lambda with no arguments. A section passes up to two
arguments according to the callback's declared parameters: the unrendered
section text and a `MustacheLambdaHelper`. A callback that declares no
parameters receives none. Any further required parameter causes PHP's usual
`ArgumentCountError`. Return a string to use as the lambda result.

The helper renders text with the section's current context:

```php
<?php
$mustache = new Mustache();
$data = [
    'name' => 'Ada',
    'upper' => function (string $text, MustacheLambdaHelper $helper): string {
        return strtoupper($helper->render($text));
    },
];

echo $mustache->render('{{#upper}}{{name}}{{/upper}}', $data), "\n";
```

Output:

```text
ADA
```

The extension creates the helper. Use it only while its callback is active.
Calling a saved helper after the callback finishes throws `MustacheException`.
PHP exceptions from callbacks propagate to the caller.

On PHP versions with Fibers, renders can overlap on one `Mustache` instance
when the templates and partials are source strings or `MustacheTemplate`
wrappers. Using an AST as the template or a partial uses shared renderer state.
A second render using that state while the first is active throws
`MustacheException`. Use separate instances for overlapping AST renders.

## Errors

For `Mustache::parse()` and `Mustache::render()`, catch errors according to
the kind of failure:

| Failure | Exception |
| --- | --- |
| Unsupported argument type, such as a resource as the entire data argument | `TypeError` |
| Invalid data, invalid partial entries, or an uninitialized template wrapper | `ValueError` |
| Invalid template or partial source | `MustacheParserException` |
| A rendering failure, such as exceeding the output limit | `MustacheException` |

`MustacheParserException` extends `MustacheException`. Its `templateLineNo`
and `templateCharNo` properties are one-based, or `-1` when unavailable.
Exceptions thrown by PHP callbacks retain their original type.

```php
<?php
$mustache = new Mustache();
try {
    $mustache->render('{{name}}', [0 => 'Ada', 'name' => 'Lin']);
} catch (ValueError $error) {
    echo "Invalid data\n";
}

echo $mustache->render('{{name}}', ['name' => 'Grace']), "\n";
```

Output:

```text
Invalid data
Grace
```

`MustacheData` has an older warning-based error contract. Constructor conversion
failures such as mixed array keys emit `E_WARNING` and leave the object
uninitialized, unless a PHP error handler throws. Calling `toValue()` on an
uninitialized instance warns and returns `false`. Passing that instance to
`render()` throws `ValueError`.

## Defaults and limits

New `Mustache` instances read these PHP INI settings:

| Setting | Default |
| --- | --- |
| `mustache.default_escape` | `1` (escaping enabled) |
| `mustache.default_start` | `{{` |
| `mustache.default_stop` | `}}` |

Changing these with `ini_set()` affects subsequently created instances.
Use `setEscapeByDefault()`, `setStartSequence()`, and `setStopSequence()` to
configure an existing instance. Delimiters must be nonempty strings.

The PHP API does not expose resource-limit setters. The extension fixes the
following limits, with MiB meaning 1,048,576 bytes:

| Operation | Limits |
| --- | --- |
| PHP data conversion | Depth 32 including the root, 100,000 values, 100,000 container entries, and 64 MiB of strings and keys in total |
| Binary AST reading and writing | 16 MiB input or output, depth 64, 100,000 nodes, 256 data parts per node, and 100,000 data parts in total |

Template parsing and rendering use the linked libmustache version's defaults.
For libmustache 0.6.0, these are:

| Operation | Limits |
| --- | --- |
| Parsing each source template | 64 MiB input, 62 nested sections, 100,000 nodes, 1 MiB per tag, and 1 KiB per delimiter |
| Rendering | 64 MiB output, nesting depth 256, 1,000,000 node visits, and 64 MiB of lambda-generated template text in total |

These limits apply to individual conversions, templates, or render operations.
There is no aggregate budget for the complete partial map.
