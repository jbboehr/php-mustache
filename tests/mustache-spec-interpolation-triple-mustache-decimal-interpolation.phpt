--TEST--
Triple Mustache Decimal Interpolation
--DESCRIPTION--
Decimals should interpolate seamlessly with proper significance.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Triple Mustache Decimal Interpolation',
  'data' => 
  array (
    'power' => 1.21,
  ),
  'expected' => '"1.21 jiggawatts!"',
  'template' => '"{{{power}}} jiggawatts!"',
  'desc' => 'Decimals should interpolate seamlessly with proper significance.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"1\.21\s*jiggawatts\!"