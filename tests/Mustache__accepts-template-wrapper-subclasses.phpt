--TEST--
Mustache accepts MustacheTemplate and MustacheAST subclasses
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
class DerivedMustacheTemplate extends MustacheTemplate
{
}

class DerivedMustacheAST extends MustacheAST
{
}

$mustache = new Mustache();
$template = new DerivedMustacheTemplate('Hello {{name}}');
$ast = new DerivedMustacheAST((string) $mustache->parse('Hello {{name}}'));

var_dump($mustache->render($template, ['name' => 'Ada']));
$templateAST = $mustache->parse($template);
var_dump($templateAST instanceof MustacheAST);
if ($templateAST instanceof MustacheAST) {
    var_dump($mustache->render($templateAST, ['name' => 'Ada']));
}
var_dump($mustache->render($ast, ['name' => 'Ada']));
var_dump($mustache->parse($ast) === $ast);

var_dump($mustache->render(
    'Before {{>value}} after',
    ['name' => 'Ada'],
    ['value' => new DerivedMustacheTemplate('{{name}}')],
));
var_dump($mustache->render(
    $mustache->parse('Before {{>value}} after'),
    ['name' => 'Ada'],
    ['value' => new DerivedMustacheAST((string) $mustache->parse('{{name}}'))],
));
?>
--EXPECT--
string(9) "Hello Ada"
bool(true)
string(9) "Hello Ada"
string(9) "Hello Ada"
bool(true)
string(16) "Before Ada after"
string(16) "Before Ada after"
