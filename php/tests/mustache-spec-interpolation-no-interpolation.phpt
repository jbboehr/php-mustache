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
  'data' => 
  array (
  ),
  'expected' => 'Hello from {Mustache}!
',
  'template' => 'Hello from {Mustache}!
',
  'desc' => 'Mustache-free templates should render as-is.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
Hello\s+from\s+\{Mustache\}\!\s+