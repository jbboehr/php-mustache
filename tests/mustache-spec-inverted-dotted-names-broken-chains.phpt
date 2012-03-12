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
  'data' => 
  array (
    'a' => 
    array (
    ),
  ),
  'expected' => '"Not Here" == "Not Here"',
  'template' => '"{{^a.b.c}}Not Here{{/a.b.c}}" == "Not Here"',
  'desc' => 'Dotted names that cannot be resolved should be considered falsey.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"Not\s*Here"\s*\=\=\s*"Not\s*Here"