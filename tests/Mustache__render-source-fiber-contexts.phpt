--TEST--
Source renders keep lambda contexts independent across interleaved Fibers
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip mustache extension required');
if (!class_exists('Fiber')) die('skip Fibers require PHP 8.1 or later');
?>
--FILE--
<?php
$mustache = new Mustache();
$source = '{{#lambda}}{{name}}{{/lambda}}|{{name}}';
$lambda = function ($text, MustacheLambdaHelper $helper) {
    $before = $helper->render($text);
    Fiber::suspend($before);
    return $before . ':' . $helper->render($text);
};
$render = function ($name) use ($mustache, $source, $lambda) {
    return $mustache->render($source, ['name' => $name, 'lambda' => $lambda]);
};
$first = new Fiber($render);
$second = new Fiber($render);

echo 'first paused: ', $first->start('Ada'), "\n";
echo 'second paused: ', $second->start('Lin'), "\n";

// Complete the first callback while the second remains suspended.
$first->resume();
echo 'first result: ', $first->getReturn(), "\n";
$second->resume();
echo 'second result: ', $second->getReturn(), "\n";

echo 'later result: ', $mustache->render($source, [
    'name' => 'Grace',
    'lambda' => function ($text, MustacheLambdaHelper $helper) {
        return 'ok:' . $helper->render($text);
    },
]), "\n";
?>
--EXPECT--
first paused: Ada
second paused: Lin
first result: Ada:Ada|Ada
second result: Lin:Lin|Lin
later result: ok:Grace|Grace
