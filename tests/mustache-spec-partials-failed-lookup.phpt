--TEST--
Failed Lookup
--DESCRIPTION--
The empty string should be used when the named partial is not found.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Failed Lookup',
  'data' => 
  array (
  ),
  'expected' => '""',
  'template' => '"{{>text}}"',
  'desc' => 'The empty string should be used when the named partial is not found.',
  'partials' => 
  array (
  ),
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""