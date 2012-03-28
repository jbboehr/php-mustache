--TEST--
Dotted Names - Falsey
--DESCRIPTION--
Dotted names should be valid for Inverted Section tags.
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
  'expected' => '"Not Here" == "Not Here"',
  'template' => '"{{^a.b.c}}Not Here{{/a.b.c}}" == "Not Here"',
  'desc' => 'Dotted names should be valid for Inverted Section tags.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"Not\s*Here"\s*\=\=\s*"Not\s*Here"