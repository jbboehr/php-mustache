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
  'data' => 
  array (
  ),
  'expected' => '12345  67890',
  'template' => '12345 {{! Comment Block! }} 67890',
  'desc' => 'Comment removal should preserve surrounding whitespace.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
12345\s*67890