--TEST--
List
--DESCRIPTION--
Lists should behave like truthy values.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'List',
  'data' => 
  array (
    'list' => 
    array (
      0 => 
      array (
        'n' => 1,
      ),
      1 => 
      array (
        'n' => 2,
      ),
      2 => 
      array (
        'n' => 3,
      ),
    ),
  ),
  'expected' => '""',
  'template' => '"{{^list}}{{n}}{{/list}}"',
  'desc' => 'Lists should behave like truthy values.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""