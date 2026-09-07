--TEST--
Mustache suspended renders retain lambda mode while later renders use new configuration
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip mustache extension required');
if (PHP_VERSION_ID < 80100) die('skip Fibers require PHP 8.1');
?>
--FILE--
<?php
if (!method_exists(Mustache::class, 'setLambdaStringMode')) die("missing lambda string mode\n");
$m = new Mustache();
$source = '{{#pause}}body{{/pause}}|{{v}}{{>force}}';
$renders = [
    'source' => [$source, []],
    'AST' => [$m->parse($source), []],
    // An AST partial forces an otherwise source-backed root onto the shared renderer.
    'AST-partial' => [$source, ['force' => $m->parse('!')]],
];
foreach ($renders as $label => [$root, $partials]) {
    $m->setLambdaStringMode(Mustache::LAMBDA_STRING_TEMPLATE);
    $fiber = new Fiber(fn () => $m->render($root, [
        'name' => 'Ada', 'v' => fn () => '{{name}}',
        'pause' => function ($text, MustacheLambdaHelper $helper) {
            Fiber::suspend($helper->render('{{v}}'));
            return $helper->render('{{v}}');
        },
    ], $partials));
    echo "$label paused:", $fiber->start(), "\n";
    $m->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);
    echo 'overlap source:', $m->render('{{v}}', ['name' => 'Lin', 'v' => fn () => '{{name}}']), "\n";
    if ($label !== 'source') {
        try { $m->render($root, [], $partials); } catch (MustacheException $e) { echo "$label overlap rejected\n"; }
    }
    $fiber->resume();
    echo 'resumed:', $fiber->getReturn(), ':configured=', $m->getLambdaStringMode(), "\n";
    echo 'later:', $m->render($root, ['name' => 'Lin', 'v' => fn () => '{{name}}'], $partials), "\n";
}
?>
--EXPECT--
source paused:Ada
overlap source:{{name}}
resumed:Ada|Ada:configured=1
later:|{{name}}
AST paused:Ada
overlap source:{{name}}
AST overlap rejected
resumed:Ada|Ada:configured=1
later:|{{name}}
AST-partial paused:Ada
overlap source:{{name}}
AST-partial overlap rejected
resumed:Ada|Ada!:configured=1
later:|{{name}}!
