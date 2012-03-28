--TEST--
Context
--DESCRIPTION--
Objects and hashes should be pushed onto the context stack.
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
  'expected' => '"Hi Joe."',
  'template' => '"{{#context}}Hi {{name}}.{{/context}}"',
  'desc' => 'Objects and hashes should be pushed onto the context stack.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"Hi\s*Joe\."