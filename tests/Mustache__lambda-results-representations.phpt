--TEST--
Explicit lambda results override both string modes across template representations without changing escaping
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip mustache extension required'); ?>
--FILE--
<?php
if (!class_exists('MustacheLiteralResult') || !class_exists('MustacheTemplateResult')) die("missing explicit lambda results\n");
$m = new Mustache();
$source = '{{>piece}}';
$body = '{{l}}|{{{l}}}|{{&l}}|{{#l}}body{{/l}}|{{t}}|{{{t}}}|{{#t}}body{{/t}}';
$literal = new MustacheLiteralResult('<b>{{name}}</b>');
$template = new MustacheTemplateResult('<b>{{name}}</b>');
$data = new MustacheData(['name' => 'Ada', 'l' => fn () => $literal, 't' => fn () => $template]);
$representations = [
    'source' => [$source, $body],
    'wrapper' => [new MustacheTemplate($source), new MustacheTemplate($body)],
    'AST-root' => [$m->parse($source), $body],
    'AST-partial' => [$source, $m->parse($body)],
    'binary-AST' => [MustacheAST::fromBinary($m->parse($source)->toBinary()),
        MustacheAST::fromBinary($m->parse($body)->toBinary())],
];
$expected = '&lt;b&gt;{{name}}&lt;/b&gt;|<b>{{name}}</b>|<b>{{name}}</b>|<b>{{name}}</b>|&lt;b&gt;Ada&lt;/b&gt;|<b>Ada</b>|<b>Ada</b>';
foreach ($representations as $label => [$root, $partial]) {
    foreach ([Mustache::LAMBDA_STRING_TEMPLATE, Mustache::LAMBDA_STRING_LITERAL] as $mode) {
        $m->setLambdaStringMode($mode);
        $actual = $m->render($root, $data, ['piece' => $partial]);
        echo "$label/$mode:", $actual === $expected ? 'ok' : $actual, "\n";
    }
}
$m->setEscapeByDefault(false);
echo $m->render('{{l}}|{{t}}', $data), "\n";
foreach ([MustacheLiteralResult::class, MustacheTemplateResult::class] as $class) {
    $m->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);
    echo "$class:", bin2hex($m->render('{{v}}', ['name' => 'Ada', 'v' => fn () => new $class("a\0{{name}}")])) , "\n";
    echo 'empty:[', $m->render('{{v}}', ['v' => fn () => new $class('')]), "]\n";
}
?>
--EXPECT--
source/0:ok
source/1:ok
wrapper/0:ok
wrapper/1:ok
AST-root/0:ok
AST-root/1:ok
AST-partial/0:ok
AST-partial/1:ok
binary-AST/0:ok
binary-AST/1:ok
<b>{{name}}</b>|<b>Ada</b>
MustacheLiteralResult:61007b7b6e616d657d7d
empty:[]
MustacheTemplateResult:6100416461
empty:[]
