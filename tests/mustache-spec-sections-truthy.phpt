--TEST--
Truthy
--DESCRIPTION--
Truthy sections should have their contents rendered.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Truthy',
  'data' => 
  array (
    'boolean' => true,
  ),
  'expected' => '"This should be rendered."',
  'template' => '"{{#boolean}}This should be rendered.{{/boolean}}"',
  'desc' => 'Truthy sections should have their contents rendered.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"This\s*should\s*be\s*rendered\."