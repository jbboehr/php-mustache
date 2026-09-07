--TEST--
Archived renders honor explicit results, helper composition, and data rejection
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip mustache extension required'); if (!method_exists(Mustache::class, 'benchmarkRenderArchive')) die('skip benchmark bridge disabled'); ?>
--FILE--
<?php
if (!class_exists('MustacheLiteralResult') || !class_exists('MustacheTemplateResult')) die("missing explicit lambda results\n");
$m = new Mustache();
$archive = $m->benchmarkSerializeArchive('{{l}}|{{{l}}}|{{t}}|{{#t}}body{{/t}}|{{#keep}}{{name}}{{/keep}}', ['piece' => '{{name}}']);
foreach ([Mustache::LAMBDA_STRING_TEMPLATE, Mustache::LAMBDA_STRING_LITERAL] as $mode) {
    $m->setLambdaStringMode($mode);
    echo "$mode:", $m->benchmarkRenderArchive($archive, [
        'name' => '{{other}}', 'other' => 'Ada',
        'l' => fn () => new MustacheLiteralResult('<b>{{name}}</b>'),
        't' => fn () => new MustacheTemplateResult('{{>piece}}'),
        'keep' => fn ($text, MustacheLambdaHelper $helper) => new MustacheLiteralResult($helper->render($text)),
    ]), "\n";
}
foreach ([MustacheLiteralResult::class, MustacheTemplateResult::class] as $class) {
    try { $m->benchmarkRenderArchive($archive, ['value' => new $class('x')]); echo "data:accepted\n"; }
    catch (ValueError $e) { echo "data:rejected\n"; }
}
?>
--EXPECT--
0:&lt;b&gt;{{name}}&lt;/b&gt;|<b>{{name}}</b>|{{other}}|{{other}}|{{other}}
1:&lt;b&gt;{{name}}&lt;/b&gt;|<b>{{name}}</b>|{{other}}|{{other}}|{{other}}
data:rejected
data:rejected
