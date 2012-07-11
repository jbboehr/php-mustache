--TEST--
List
--DESCRIPTION--
Lists should be iterated; list items should visit the context stack.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'List',
  'desc' => 'Lists should be iterated; list items should visit the context stack.',
  'data' => 
  array (
    'list' => 
    array (
      0 => 
      array (
        'item' => 1,
      ),
      1 => 
      array (
        'item' => 2,
      ),
      2 => 
      array (
        'item' => 3,
      ),
    ),
  ),
  'template' => '"{{#list}}{{item}}{{/list}}"',
  'expected' => '"123"',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"123"