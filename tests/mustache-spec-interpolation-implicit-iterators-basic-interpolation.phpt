--TEST--
Implicit Iterators - Basic Interpolation
--DESCRIPTION--
Unadorned tags should interpolate content into the template.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Implicit Iterators - Basic Interpolation',
  'desc' => 'Unadorned tags should interpolate content into the template.',
  'data' => 'world',
  'template' => 'Hello, {{.}}!
',
  'expected' => 'Hello, world!
',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>Hello, world!
</render>
