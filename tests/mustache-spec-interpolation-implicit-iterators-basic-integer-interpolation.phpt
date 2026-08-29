--TEST--
Implicit Iterators - Basic Integer Interpolation
--DESCRIPTION--
Integers should interpolate seamlessly.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Implicit Iterators - Basic Integer Interpolation',
  'desc' => 'Integers should interpolate seamlessly.',
  'data' => 85,
  'template' => '"{{.}} miles an hour!"',
  'expected' => '"85 miles an hour!"',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>"85 miles an hour!"</render>
