--TEST--
Dotted Names - Broken Chain Resolution
--DESCRIPTION--
Each part of a dotted name should resolve only against its parent.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Dotted Names - Broken Chain Resolution',
  'desc' => 'Each part of a dotted name should resolve only against its parent.',
  'data' => 
  array (
    'a' => 
    array (
      'b' => 
      array (
      ),
    ),
    'c' => 
    array (
      'name' => 'Jim',
    ),
  ),
  'template' => '"{{a.b.c.name}}" == ""',
  'expected' => '"" == ""',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""\s*\=\=\s*""