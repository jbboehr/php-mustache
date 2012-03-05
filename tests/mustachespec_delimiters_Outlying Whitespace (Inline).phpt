--TEST--
Outlying Whitespace (Inline)
--DESCRIPTION--
Whitespace should be left untouched.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Outlying Whitespace (Inline)',
  'data' => 
  array (
  ),
  'expected' => ' | 
',
  'template' => ' | {{=@ @=}}
',
  'desc' => 'Whitespace should be left untouched.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s+\|\s+