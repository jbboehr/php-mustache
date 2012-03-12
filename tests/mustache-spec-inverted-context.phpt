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
  'data' => 
  array (
    'context' => 
    array (
      'name' => 'Joe',
    ),
  ),
  'expected' => '""',
  'template' => '"{{^context}}Hi {{name}}.{{/context}}"',
  'desc' => 'Objects and hashes should behave like truthy values.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""