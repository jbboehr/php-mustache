--TEST--
Falsey
--DESCRIPTION--
Falsey sections should have their contents omitted.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Falsey',
  'desc' => 'Falsey sections should have their contents omitted.',
  'data' => 
  array (
    'boolean' => false,
  ),
  'template' => '"{{#boolean}}This should not be rendered.{{/boolean}}"',
  'expected' => '""',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""