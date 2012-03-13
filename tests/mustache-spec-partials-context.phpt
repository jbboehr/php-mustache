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
  'data' => 
  array (
    'text' => 'content',
  ),
  'expected' => '"*content*"',
  'template' => '"{{>partial}}"',
  'desc' => 'The greater-than operator should operate within the current context.',
  'partials' => 
  array (
    'partial' => '*{{text}}*',
  ),
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
"\*content\*"