# PHP API guide

Use the [official Mustache manual](https://mustache.github.io/mustache.5.html)
for template syntax, sections, escaping, and partials. This guide covers how
the PHP extension accepts data and calls PHP code. The
[PHP stub](../mustache.stub.php) lists public methods, types, and constants.

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

Binary persistence cannot preserve all section metadata. Both `toBinary()` and
PHP's `serialize($ast)` throw `MustacheException` for ASTs containing a `#` section
with either of these properties:

- Delimiters other than `{{` and `}}`: the message is
  `Legacy serialization cannot preserve custom section delimiters`.
- Original body text that differs from the binary format's reconstructed text:
  the message is `Legacy serialization cannot preserve original section text`.
  Tag spacing, comments, triple-brace spelling, and standalone formatting can
  cause this difference, even with default delimiters.

These ASTs still render normally and remain usable after the exception. Cache
their source and parse it when needed to preserve exact section callback text.
Changing delimiters for interpolation alone does not prevent binary persistence.

AST partials preserve the delimiters and escaping setting used when they were
parsed, including when rendered by another `Mustache` instance. ASTs parsed from
source retain that source and parse it again when copied into a partial map to
preserve exact section callback text. This adds source storage and parsing work
for each render that uses them as partials.

An empty string or `new MustacheTemplate('')` is a valid empty template.
`new MustacheTemplate()` and `new MustacheTemplate(null)` leave their wrappers
uninitialized. Passing these to `parse()` or `render()` throws `ValueError`.
`new MustacheAST()` and `new MustacheAST(null)` throw `MustacheException`
because empty input is not a valid binary AST. Use `parse('')` for an empty AST.
An AST whose constructor was bypassed is uninitialized and throws `ValueError`
from `parse()`, `render()`, `toArray()`, and `toBinary()`.

### Inspecting node types and comments

`Mustache::tokenize($source)` and `MustacheAST::toArray()` expose libmustache's
diagnostic tree. Compare a node's `type` with these public integer constants:

| Constant | Node kind |
| --- | --- |
| `MustacheAST::NODE_NONE` | Unset node |
| `MustacheAST::NODE_ROOT` | Template root |
| `MustacheAST::NODE_OUTPUT` | Literal output |
| `MustacheAST::NODE_TAG` | Generic tag |
| `MustacheAST::NODE_CONTAINER` | Generic container |
| `MustacheAST::NODE_VARIABLE` | Escaped or unescaped interpolation |
| `MustacheAST::NODE_NEGATE` | Inverted section |
| `MustacheAST::NODE_SECTION` | Section |
| `MustacheAST::NODE_STOP` | Closing section tag |
| `MustacheAST::NODE_COMMENT` | Comment |
| `MustacheAST::NODE_PARTIAL` | Partial inclusion |
| `MustacheAST::NODE_INLINE_PARTIAL` | Inline partial node |

These constants mirror the individual libmustache node-type enum values.
Some kinds are used internally or by legacy trees and may not occur when
parsing source. The constants do not enable additional template syntax or
expose the library's combined type masks or node flags.

For example, walk the tree to extract comment contents before rendering:

```php
<?php
function templateComments(array $node): iterable
{
    if ($node['type'] === MustacheAST::NODE_COMMENT) {
        yield $node['data'] ?? '';
    }
    foreach ($node['children'] ?? [] as $child) {
        yield from templateComments($child);
    }
}

$ast = (new Mustache())->parse('{{! asset: main.css }}Hello');
foreach (templateComments($ast->toArray()) as $comment) {
    echo $comment, "\n";
}
```

Output:

```text
asset: main.css
```

Comment contents have surrounding whitespace trimmed. Empty comments may
omit `data`, which is why the example uses `?? ''`. Comments produce no rendered
output. The constants provide names for node kinds, but the diagnostic array
shape and other fields remain outside the compatibility contract. Use source
or the supported binary APIs for caches. For application metadata that needs
an independent format, preprocess front matter or store metadata separately.

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

Conversion captures each container's immediate values before converting its
children. A lazy initializer changing a referenced scalar therefore does not
change that scalar's already captured value. Nested objects keep their identity,
and their properties are collected when conversion reaches them, so those later
property values can reflect changes made by earlier initializers. Conversion
does not make an atomic copy of the entire object graph.

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

For initialized objects with
[PHP 8.4+ property hooks](https://www.php.net/manual/en/language.oop5.property-hooks.php),
property collection reads stored backing values without invoking `get` hooks.
Virtual properties and uninitialized backing values are omitted. For example,
a getter that uppercases a stored `Ada` still renders `Ada` when the object is
passed directly. To supply computed values, read the properties explicitly
into an array, such as `['name' => $person->name]`, and pass that array as data.

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
`ArgumentCountError`. Return a string or an explicit result object as described
below.

### Callback string interpretation

By default, callback strings are evaluated as template source. Use
`setLambdaStringMode()` to display their contents without that evaluation:

```php
<?php
$mustache = new Mustache();
$mustache->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);
echo $mustache->render('{{value}}', [
    'name' => 'Ada',
    'value' => fn (): string => 'Hello {{name}}',
]), "\n";
// Hello {{name}}
```

`Mustache::LAMBDA_STRING_TEMPLATE` restores template evaluation and is the
default for new instances. `getLambdaStringMode()` returns the configured
constant. An unknown integer mode throws `ValueError` and leaves the setting
unchanged; arguments otherwise follow PHP's usual integer parameter rules.
The setting also applies after the existing conversion of scalar or stringable
callback returns, including `MustacheTemplate` objects.

Literal mode preserves the template's escaping rules: escaped interpolation
still escapes HTML, while triple-brace and ampersand tags remain unescaped.
Section results receive no additional section-wide escaping. Literal mode does
not mark text as HTML-safe. In template mode, returned interpolation templates
use default delimiters and returned section templates use the section's opening
delimiters. The complete evaluated interpolation still follows the outer tag's
escaping rules.

Each render captures its mode before data conversion, template property reads,
or rendering callbacks invoke PHP. Changes made during a render affect later
calls, including nested calls, while the active render and its section helpers
retain their original mode. The getter reports the configured value for later
calls. Parsed templates and `MustacheData` values can be reused under either
mode; interpretation is selected when rendering.

### Explicit callback results

Return `MustacheLiteralResult` to display text without template evaluation, or
`MustacheTemplateResult` to request evaluation. These choices override the
engine's string mode for that callback result:

```php
<?php
$mustache = new Mustache();
$mustache->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);
echo $mustache->render('{{literal}} | {{template}}', [
    'name' => 'Ada',
    'literal' => fn (): MustacheLiteralResult => new MustacheLiteralResult('Hello {{name}}'),
    'template' => fn (): MustacheTemplateResult => new MustacheTemplateResult('Hello {{name}}'),
]), "\n";
// Hello {{name}} | Hello Ada
```

Both classes are final and immutable. Construct them with `new ...($text)` and
read the original text with `getText(): string`. Text may be empty or contain
NUL bytes. Each result owns its text, can be retained and returned repeatedly,
and keeps no engine or section helper alive. `clone` produces a distinct object
with the same text; `==` compares the class and exact text bytes, while `===`
compares object identity. Calling the constructor again throws `Error`, and
dynamic properties are rejected. PHP serialization and construction through
`ReflectionClass::newInstanceWithoutConstructor()` are also rejected. To persist
a result, store its text and reconstruct the intended class.

These objects are accepted as callback returns, including returns by reference.
Passing one directly in render data, to `MustacheData`, or to
`debugDataStructure()` throws `ValueError`; wrap it in a callback instead.
They do not implement `__toString()`. `MustacheTemplate` remains the existing
root/partial source wrapper, and returning one from a callback still follows
ordinary string conversion and the engine's string mode.

Explicit results preserve the escaping and delimiter rules described above.
For example, `new MustacheLiteralResult('<b>{{name}}</b>')` returned from an
escaped interpolation produces `&lt;b&gt;{{name}}&lt;/b&gt;`. It does not mark
the text as HTML-safe. Explicit template results can throw
`MustacheParserException` if their source is invalid when evaluated.

### Section helpers

For templates parsed from source, the section body preserves the original tag
spelling, comments, and whitespace.

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

The helper returns a string. In template mode, returning that string from a
callback evaluates it again. In literal mode, it is returned without another
template evaluation, preserving any Mustache-like text in the rendered data.

Wrap helper output in `MustacheLiteralResult` to preserve it under either
string mode:

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
Exceptions thrown by PHP callbacks or error handlers propagate unchanged.
If an exception escapes a rendering callback, later template callbacks are
not invoked.

Invalid array key combinations and limits reached while collecting a container
are reported before its children are converted. These errors can therefore
precede an exception that a child's lazy initializer would have thrown.

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

The extension fixes the following data-conversion and binary-AST limits,
with MiB meaning 1,048,576 bytes:

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
Aggregate partial-map limits are disabled by default and can be enabled on
individual `Mustache` instances.

### Partial-map limits

`setPartialLimits(?int $maxEntries = null, ?int $maxTextBytes = null): void`
replaces both limits for subsequent renders. For example:

```php
<?php
$mustache = new Mustache();
$mustache->setPartialLimits(maxEntries: 2, maxTextBytes: 32);
echo $mustache->render(
    '{{>greeting}}',
    ['name' => 'Ada'],
    ['greeting' => 'Hello {{name}}']
), "\n";
```

This prints `Hello Ada`. The limits in this example are application choices.
Choose limits appropriate to your template catalog.

`null` disables the corresponding aggregate limit. Zero is a real zero
allowance. A negative value throws `ValueError` and leaves both previous limits
unchanged. Omitted arguments default to `null`, so `setPartialLimits()` disables
both limits and `setPartialLimits(maxTextBytes: 32)` also disables the entry
limit. The existing per-template limits still apply.

`maxEntries` counts every supplied map entry, including unused entries. The
same object supplied under two names counts twice. `maxTextBytes` counts each
map name plus the following text for its value:

| Partial value | Text charged to the budget |
| --- | --- |
| String | Source bytes |
| `MustacheTemplate` | Source bytes returned by its `template` property |
| AST created by `parse()` | Retained original source bytes used for reparsing |
| AST loaded from binary | Copied node data, dotted-name parts, delimiter strings, and nested partial names |

Lengths are byte counts, including embedded NULs. Binary AST fields are counted
separately even when their contents repeat. Different representations can
therefore consume different allowances. Each named occurrence is charged again.
The root template and render data have their own limits and are not charged to
this partial-map budget.

Preparation checks each entry before copying its name or source, reparsing its
AST, or cloning it. Wrapper properties are read once, in the existing preparation
order. Earlier entries may already have been prepared when a later entry exceeds
a limit. The call then throws `ValueError`, releases prepared entries, and returns
no output. The same instance can render again with a fresh allowance.

Each render captures both settings before data conversion or template-property
callbacks. Changing them during a callback affects later or nested renders,
while an already active or suspended render keeps its captured settings.

These limits bound entry count and text, not total memory or aggregate node
count. They do not include node/container overhead or prevent PHP callbacks from
allocating the values they return.
