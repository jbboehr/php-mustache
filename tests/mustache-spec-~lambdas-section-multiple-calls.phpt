--TEST--
Section - Multiple Calls
--DESCRIPTION--
Lambdas used for sections should not be cached.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Section - Multiple Calls',
  'desc' => 'Lambdas used for sections should not be cached.',
  'data' => 
  array (
    'lambda' => function ($text = '') { return "__" . $text . "__"; },
  ),
  'template' => '{{#lambda}}FILE{{/lambda}} != {{#lambda}}LINE{{/lambda}}',
  'expected' => '__FILE__ != __LINE__',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
__FILE__\s*\!\=\s*__LINE__