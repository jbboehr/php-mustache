--TEST--
Surrounding Whitespace
--DESCRIPTION--
Comment removal should preserve surrounding whitespace.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Surrounding Whitespace',
  'desc' => 'Comment removal should preserve surrounding whitespace.',
  'data' => 
  array (
  ),
  'template' => '12345 {{! Comment Block! }} 67890',
  'expected' => '12345  67890',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
12345\s*67890