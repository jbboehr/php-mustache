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
  'data' => 
  array (
  ),
  'expected' => '1234567890
',
  'template' => '12345{{!
  This is a
  multi-line comment...
}}67890
',
  'desc' => 'Multiline comments should be permitted.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
1234567890\s*