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
  'data' => 
  array (
    'a' => 
    array (
    ),
  ),
  'expected' => '"" == ""',
  'template' => '"{{a.b.c}}" == ""',
  'desc' => 'Any falsey value prior to the last part of the name should yield \'\'.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
""\s*\=\=\s*""