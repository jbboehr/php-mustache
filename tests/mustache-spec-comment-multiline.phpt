--TEST--
Multiline
--DESCRIPTION--
Multiline comments should be permitted.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Multiline',
  'desc' => 'Multiline comments should be permitted.',
  'data' => 
  array (
  ),
  'template' => '12345{{!
  This is a
  multi-line comment...
}}67890
',
  'expected' => '1234567890
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
1234567890\s*