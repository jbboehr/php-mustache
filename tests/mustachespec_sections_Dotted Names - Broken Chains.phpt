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
  'expected' => '"" == ""',
  'template' => '"{{#a.b.c}}Here{{/a.b.c}}" == ""',
  'desc' => 'Dotted names that cannot be resolved should be considered falsey.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""\s+\=\=\s+""