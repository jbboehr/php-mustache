# Issue 68 resolution draft

Local draft for [issue #68](https://github.com/jbboehr/php-mustache/issues/68).
The implementation is on `develop` and is unreleased. This text has not been
posted, and the issue has not been closed. See the
[remaining release work](README.md#remaining-planned-work) and
[binding compatibility notes](libmustache-compatibility.md#callback-results).

## Draft comment

The current `develop` branch provides two ways to return computed text from a
PHP lambda without evaluating that text as another Mustache template. These
APIs are not yet in a release.

To apply this behavior to ordinary callback strings on a `Mustache` instance,
enable literal mode:

```php
<?php
$mustache = new Mustache();
$mustache->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);
echo $mustache->render('{{value}} | {{{value}}}', [
    'name' => 'Ada',
    'value' => fn (): string => '<b>{{name}}</b>',
]), "\n";
// &lt;b&gt;{{name}}&lt;/b&gt; | <b>{{name}}</b>
```

The braces in the callback output are preserved. Escaping still follows the
outer tag: `{{value}}` escapes HTML, while `{{{value}}}` and `{{&value}}` leave
it unescaped. Literal mode does not make a value HTML-safe.

To choose interpretation for an individual callback, return
`MustacheLiteralResult` or `MustacheTemplateResult`. Each class overrides the
engine's string mode for that result:

```php
<?php
$mustache = new Mustache();
echo $mustache->render('{{literal}} | {{template}}', [
    'name' => 'Ada',
    'literal' => fn (): MustacheLiteralResult => new MustacheLiteralResult('Hello {{name}}'),
    'template' => fn (): MustacheTemplateResult => new MustacheTemplateResult('Hello {{name}}'),
]), "\n";
// Hello {{name}} | Hello Ada
```

Both classes are final and hold immutable text. Return them from callbacks,
including section callbacks. To preserve text already rendered by a section
helper, return `new MustacheLiteralResult($helper->render($text))`.

Existing applications keep the current default: ordinary callback strings are
evaluated as template source. Literal behavior is opt-in through the setting
or the result class. `Mustache::LAMBDA_STRING_TEMPLATE` restores template mode.
The [PHP API guide](https://github.com/jbboehr/php-mustache/blob/develop/docs/php-api.md#php-lambdas)
describes both APIs and their escaping, delimiter, and helper behavior.
