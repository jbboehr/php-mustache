--TEST--
Ampersand Decimal Interpolation
--DESCRIPTION--
Decimals should interpolate seamlessly with proper significance.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Ampersand Decimal Interpolation',
  'desc' => 'Decimals should interpolate seamlessly with proper significance.',
  'data' => 
  array (
    'power' => 1.21,
  ),
  'template' => '"{{&power}} jiggawatts!"',
  'expected' => '"1.21 jiggawatts!"',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"1\.21\s*jiggawatts\!"