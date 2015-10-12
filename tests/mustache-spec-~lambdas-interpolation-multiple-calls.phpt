--TEST--
Interpolation - Multiple Calls
--DESCRIPTION--
Interpolated lambdas should not be cached.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Interpolation - Multiple Calls',
  'desc' => 'Interpolated lambdas should not be cached.',
  'data' => 
  array (
    'lambda' => function ($text = '') { global $calls; return ++$calls; },
  ),
  'template' => '{{lambda}} == {{{lambda}}} == {{lambda}}',
  'expected' => '1 == 2 == 3',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
1\s*\=\=\s*2\s*\=\=\s*3