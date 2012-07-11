--TEST--
Dotted Names - Broken Chains
--DESCRIPTION--
Any falsey value prior to the last part of the name should yield ''.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Dotted Names - Broken Chains',
  'desc' => 'Any falsey value prior to the last part of the name should yield \'\'.',
  'data' => 
  array (
    'a' => 
    array (
    ),
  ),
  'template' => '"{{a.b.c}}" == ""',
  'expected' => '"" == ""',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""\s*\=\=\s*""