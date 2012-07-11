--TEST--
Dotted Names - Truthy
--DESCRIPTION--
Dotted names should be valid for Inverted Section tags.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Dotted Names - Truthy',
  'desc' => 'Dotted names should be valid for Inverted Section tags.',
  'data' => 
  array (
    'a' => 
    array (
      'b' => 
      array (
        'c' => true,
      ),
    ),
  ),
  'template' => '"{{^a.b.c}}Not Here{{/a.b.c}}" == ""',
  'expected' => '"" == ""',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""\s*\=\=\s*""