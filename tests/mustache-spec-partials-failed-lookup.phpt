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
  'desc' => 'The empty string should be used when the named partial is not found.',
  'data' => 
  array (
  ),
  'template' => '"{{>text}}"',
  'partials' => 
  array (
  ),
  'expected' => '""',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>""</render>