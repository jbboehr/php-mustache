--TEST--
Explicit lambda result aliases preserve value comparison and callback interpretation
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip mustache extension required');
if (PHP_VERSION_ID < 80300) die('skip aliases for internal classes require PHP 8.3');
?>
--FILE--
<?php
if (!class_exists('MustacheLiteralResult') || !class_exists('MustacheTemplateResult')) die("missing explicit lambda results\n");
class_alias(MustacheLiteralResult::class, 'LiteralResultAlias');
class_alias(MustacheTemplateResult::class, 'TemplateResultAlias');

$literal = new LiteralResultAlias('{{name}}');
$template = new TemplateResultAlias('{{name}}');
var_dump(
    $literal == new MustacheLiteralResult('{{name}}'),
    $template == new MustacheTemplateResult('{{name}}'),
    $literal == $template,
);

$m = new Mustache();
foreach ([Mustache::LAMBDA_STRING_TEMPLATE, Mustache::LAMBDA_STRING_LITERAL] as $mode) {
    $m->setLambdaStringMode($mode);
    echo "$mode:", $m->render('{{literal}}|{{template}}', [
        'name' => 'Ada', 'literal' => fn () => $literal, 'template' => fn () => $template,
    ]), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
0:{{name}}|Ada
1:{{name}}|Ada
