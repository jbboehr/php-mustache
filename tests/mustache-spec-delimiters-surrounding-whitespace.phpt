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
  'data' => 
  array (
  ),
  'expected' => '|  |',
  'template' => '| {{=@ @=}} |',
  'desc' => 'Surrounding whitespace should be left untouched.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\s*\|