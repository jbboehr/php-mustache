--TEST--
AST renders reject Fiber overlap and preserve the active and later renders
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip mustache extension required');
if (!class_exists('Fiber')) die('skip Fibers require PHP 8.1 or later');
?>
--FILE--
<?php
$mustache = new Mustache();
$ast = $mustache->parse('{{#lambda}}{{name}}{{/lambda}}|{{name}}');
$first = new Fiber(function () use ($mustache, $ast) {
    return $mustache->render($ast, [
        'name' => 'Ada',
        'lambda' => function ($text, MustacheLambdaHelper $helper) {
            $before = $helper->render($text);
            Fiber::suspend($before);
            return $before . ':' . $helper->render($text);
        },
    ]);
});
$overlapCalls = 0;
$second = new Fiber(function () use ($mustache, $ast, &$overlapCalls) {
    return $mustache->render($ast, [
        'name' => 'Lin',
        'lambda' => function ($text, MustacheLambdaHelper $helper) use (&$overlapCalls) {
            $overlapCalls++;
            return $helper->render($text);
        },
    ]);
});

echo 'first paused: ', $first->start(), "\n";
try {
    $second->start();
    echo "overlapping render was accepted\n";
} catch (MustacheException $e) {
    echo $e->getMessage(), "\n";
}
echo 'overlapping callback calls: ', $overlapCalls, "\n";

$first->resume();
echo 'first result: ', $first->getReturn(), "\n";
echo 'later result: ', $mustache->render($ast, [
    'name' => 'Grace',
    'lambda' => function ($text, MustacheLambdaHelper $helper) {
        return 'ok:' . $helper->render($text);
    },
]), "\n";
?>
--EXPECT--
first paused: Ada
Renderer is already rendering
overlapping callback calls: 0
first result: Ada:Ada|Ada
later result: ok:Grace|Grace
