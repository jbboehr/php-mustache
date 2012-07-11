--TEST--
No Interpolation
--DESCRIPTION--
Mustache-free templates should render as-is.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'No Interpolation',
  'desc' => 'Mustache-free templates should render as-is.',
  'data' => 
  array (
  ),
  'template' => 'Hello from {Mustache}!
',
  'expected' => 'Hello from {Mustache}!
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
Hello\s*from\s*\{Mustache\}\!\s*