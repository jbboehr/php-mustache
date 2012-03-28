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
  'data' => 
  array (
  ),
  'expected' => '1234567890',
  'template' => '12345{{! Comment Block! }}67890',
  'desc' => 'Comment blocks should be removed from the template.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
1234567890