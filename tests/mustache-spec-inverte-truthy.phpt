--TEST--
Truthy
--DESCRIPTION--
Truthy sections should have their contents omitted.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Truthy',
  'desc' => 'Truthy sections should have their contents omitted.',
  'data' => 
  array (
    'boolean' => true,
  ),
  'template' => '"{{^boolean}}This should not be rendered.{{/boolean}}"',
  'expected' => '""',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""