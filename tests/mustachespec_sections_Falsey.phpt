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
  'data' => 
  array (
    'boolean' => false,
  ),
  'expected' => '""',
  'template' => '"{{#boolean}}This should not be rendered.{{/boolean}}"',
  'desc' => 'Falsey sections should have their contents omitted.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""