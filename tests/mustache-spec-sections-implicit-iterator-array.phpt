--TEST--
Implicit Iterator - Array
--DESCRIPTION--
Implicit iterators should allow iterating over nested arrays.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Implicit Iterator - Array',
  'desc' => 'Implicit iterators should allow iterating over nested arrays.',
  'data' => 
  array (
    'list' => 
    array (
      0 => 
      array (
        0 => 1,
        1 => 2,
        2 => 3,
      ),
      1 => 
      array (
        0 => 'a',
        1 => 'b',
        2 => 'c',
      ),
    ),
  ),
  'template' => '"{{#list}}({{#.}}{{.}}{{/.}}){{/list}}"',
  'expected' => '"(123)(abc)"',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>"(123)(abc)"</render>
