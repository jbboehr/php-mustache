--TEST--
Context
--DESCRIPTION--
The greater-than operator should operate within the current context.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Context',
  'desc' => 'The greater-than operator should operate within the current context.',
  'data' => 
  array (
    'text' => 'content',
  ),
  'template' => '"{{>partial}}"',
  'partials' => 
  array (
    'partial' => '*{{text}}*',
  ),
  'expected' => '"*content*"',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
"\*content\*"