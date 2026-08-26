--TEST--
Surrounding Whitespace
--DESCRIPTION--
Surrounding whitespace should be left untouched.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Surrounding Whitespace',
  'desc' => 'Surrounding whitespace should be left untouched.',
  'data' => 
  array (
  ),
  'template' => '| {{=@ @=}} |',
  'expected' => '|  |',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>|  |</render>