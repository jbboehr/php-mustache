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
  'desc' => 'Whitespace should be left untouched.',
  'data' => 
  array (
  ),
  'template' => ' | {{=@ @=}}
',
  'expected' => ' | 
',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render> | 
</render>