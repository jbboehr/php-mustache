--TEST--
Ampersand Integer Interpolation
--DESCRIPTION--
Integers should interpolate seamlessly.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Ampersand Integer Interpolation',
  'data' => 
  array (
    'mph' => 85,
  ),
  'expected' => '"85 miles an hour!"',
  'template' => '"{{&mph}} miles an hour!"',
  'desc' => 'Integers should interpolate seamlessly.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"85\s*miles\s*an\s*hour\!"