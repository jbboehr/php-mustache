--TEST--
Interpolation
--DESCRIPTION--
A lambda's return value should be interpolated.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Interpolation',
  'desc' => 'A lambda\'s return value should be interpolated.',
  'data' => 
  array (
    'lambda' => function ($text = '') { return "world"; },
  ),
  'template' => 'Hello, {{lambda}}!',
  'expected' => 'Hello, world!',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
Hello,\s*world\!