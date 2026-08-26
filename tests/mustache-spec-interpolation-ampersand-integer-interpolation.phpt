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
  'desc' => 'Integers should interpolate seamlessly.',
  'data' => 
  array (
    'mph' => 85,
  ),
  'template' => '"{{&mph}} miles an hour!"',
  'expected' => '"85 miles an hour!"',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>"85 miles an hour!"</render>