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
  'desc' => 'Objects and hashes should be pushed onto the context stack.',
  'data' => 
  array (
    'context' => 
    array (
      'name' => 'Joe',
    ),
  ),
  'template' => '"{{#context}}Hi {{name}}.{{/context}}"',
  'expected' => '"Hi Joe."',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>"Hi Joe."</render>