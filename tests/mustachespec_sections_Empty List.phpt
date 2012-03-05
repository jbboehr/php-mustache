--TEST--
Empty List
--DESCRIPTION--
Empty lists should behave like falsey values.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Empty List',
  'data' => 
  array (
    'list' => 
    array (
    ),
  ),
  'expected' => '""',
  'template' => '"{{#list}}Yay lists!{{/list}}"',
  'desc' => 'Empty lists should behave like falsey values.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""