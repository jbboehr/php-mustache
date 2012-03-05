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
  'data' => 
  array (
    'boolean' => true,
  ),
  'expected' => '""',
  'template' => '"{{^boolean}}This should not be rendered.{{/boolean}}"',
  'desc' => 'Truthy sections should have their contents omitted.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""