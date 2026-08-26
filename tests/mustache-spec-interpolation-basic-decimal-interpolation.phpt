--TEST--
Basic Decimal Interpolation
--DESCRIPTION--
Decimals should interpolate seamlessly with proper significance.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Basic Decimal Interpolation',
  'desc' => 'Decimals should interpolate seamlessly with proper significance.',
  'data' => 
  array (
    'power' => 1.21,
  ),
  'template' => '"{{power}} jiggawatts!"',
  'expected' => '"1.21 jiggawatts!"',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>"1.21 jiggawatts!"</render>