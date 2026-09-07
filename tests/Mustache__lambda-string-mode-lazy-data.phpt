--TEST--
Mustache snapshots lambda mode before initializing lazy render data
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip mustache extension required');
if (PHP_VERSION_ID < 80400) die('skip lazy objects require PHP 8.4');
?>
--FILE--
<?php
if (!method_exists(Mustache::class, 'setLambdaStringMode')) die("missing lambda string mode\n");
class ModeData { public string $name; public Closure $v; }
$m = new Mustache();
$renders = ['source' => fn ($data) => $m->render('{{v}}', $data)];
$ast = $m->parse('{{v}}');
$renders['AST'] = fn ($data) => $m->render($ast, $data);
foreach ($renders as $label => $render) {
    $m->setLambdaStringMode(Mustache::LAMBDA_STRING_TEMPLATE);
    $data = (new ReflectionClass(ModeData::class))->newLazyGhost(function ($data) use ($m) {
        $m->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);
        $data->name = 'Ada';
        $data->v = fn () => '{{name}}';
    });
    echo "$label:", $render($data), ':later=', $render($data), "\n";
}
?>
--EXPECT--
source:Ada:later={{name}}
AST:Ada:later={{name}}
