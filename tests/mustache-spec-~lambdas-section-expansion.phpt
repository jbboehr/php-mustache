--TEST--
Section - Expansion
--DESCRIPTION--
Lambdas used for sections should have their results parsed.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Section - Expansion',
  'desc' => 'Lambdas used for sections should have their results parsed.',
  'data' => 
  array (
    'planet' => 'Earth',
    'lambda' => function ($text = '') { return $text . "{{planet}}" . $text; },
  ),
  'template' => '<{{#lambda}}-{{/lambda}}>',
  'expected' => '<-Earth->',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\<\-Earth\-\>