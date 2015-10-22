--TEST--
Inverted Section
--DESCRIPTION--
Lambdas used for inverted sections should be considered truthy.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Inverted Section',
  'desc' => 'Lambdas used for inverted sections should be considered truthy.',
  'data' => 
  array (
    'static' => 'static',
    'lambda' => function ($text = '') { return false; },
  ),
  'template' => '<{{^lambda}}{{static}}{{/lambda}}>',
  'expected' => '<>',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\<\>