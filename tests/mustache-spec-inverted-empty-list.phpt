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
  'desc' => 'Empty lists should behave like falsey values.',
  'data' => 
  array (
    'list' => 
    array (
    ),
  ),
  'template' => '"{{^list}}Yay lists!{{/list}}"',
  'expected' => '"Yay lists!"',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"Yay\s*lists\!"