--TEST--
Falsey
--DESCRIPTION--
Falsey sections should have their contents rendered.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Falsey',
  'desc' => 'Falsey sections should have their contents rendered.',
  'data' => 
  array (
    'boolean' => false,
  ),
  'template' => '"{{^boolean}}This should be rendered.{{/boolean}}"',
  'expected' => '"This should be rendered."',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"This\s*should\s*be\s*rendered\."