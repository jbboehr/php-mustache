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
  'desc' => 'Lists should behave like truthy values.',
  'data' => 
  array (
    'list' => 
    array (
      0 => 
      array (
        '' => 1,
      ),
      1 => 
      array (
        '' => 2,
      ),
      2 => 
      array (
        '' => 3,
      ),
    ),
  ),
  'template' => '"{{^list}}{{n}}{{/list}}"',
  'expected' => '""',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""