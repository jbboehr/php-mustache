--TEST--
Inline
--DESCRIPTION--
Comment blocks should be removed from the template.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Inline',
  'desc' => 'Comment blocks should be removed from the template.',
  'data' => 
  array (
  ),
  'template' => '12345{{! Comment Block! }}67890',
  'expected' => '1234567890',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
1234567890