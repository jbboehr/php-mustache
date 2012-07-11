--TEST--
Context
--DESCRIPTION--
Objects and hashes should behave like truthy values.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Context',
  'desc' => 'Objects and hashes should behave like truthy values.',
  'data' => 
  array (
    'context' => 
    array (
      'name' => 'Joe',
    ),
  ),
  'template' => '"{{^context}}Hi {{name}}.{{/context}}"',
  'expected' => '""',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""