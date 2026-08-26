--TEST--
Interpolation - Expansion
--DESCRIPTION--
A lambda's return value should be parsed.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Interpolation - Expansion',
  'desc' => 'A lambda\'s return value should be parsed.',
  'data' => 
  array (
    'planet' => 'world',
    'lambda' => function ($text = '') { return "{{planet}}"; },
  ),
  'template' => 'Hello, {{lambda}}!',
  'expected' => 'Hello, world!',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>Hello, world!</render>