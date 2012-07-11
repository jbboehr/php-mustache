--TEST--
Dotted Names - Broken Chains
--DESCRIPTION--
Dotted names that cannot be resolved should be considered falsey.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Dotted Names - Broken Chains',
  'desc' => 'Dotted names that cannot be resolved should be considered falsey.',
  'data' => 
  array (
    'a' => 
    array (
    ),
  ),
  'template' => '"{{^a.b.c}}Not Here{{/a.b.c}}" == "Not Here"',
  'expected' => '"Not Here" == "Not Here"',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"Not\s*Here"\s*\=\=\s*"Not\s*Here"