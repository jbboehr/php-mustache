--TEST--
Section
--DESCRIPTION--
Lambdas used for sections should receive the raw section string.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Section',
  'desc' => 'Lambdas used for sections should receive the raw section string.',
  'data' => 
  array (
    'x' => 'Error!',
    'lambda' => function ($text = '') { return ($text == "{{x}}") ? "yes" : "no"; },
  ),
  'template' => '<{{#lambda}}{{x}}{{/lambda}}>',
  'expected' => '<yes>',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\<yes\>