--TEST--
Dotted Names - Falsey
--DESCRIPTION--
Dotted names should be valid for Section tags.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Dotted Names - Falsey',
  'data' => 
  array (
    'a' => 
    array (
      'b' => 
      array (
        'c' => false,
      ),
    ),
  ),
  'expected' => '"" == ""',
  'template' => '"{{#a.b.c}}Here{{/a.b.c}}" == ""',
  'desc' => 'Dotted names should be valid for Section tags.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""\s*\=\=\s*""