--TEST--
Basic Behavior
--DESCRIPTION--
The greater-than operator should expand to the named partial.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Basic Behavior',
  'data' => 
  array (
  ),
  'expected' => '"from partial"',
  'template' => '"{{>text}}"',
  'desc' => 'The greater-than operator should expand to the named partial.',
  'partials' => 
  array (
    'text' => 'from partial',
  ),
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
"from\s*partial"