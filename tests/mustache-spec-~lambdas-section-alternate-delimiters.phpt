--TEST--
Section - Alternate Delimiters
--DESCRIPTION--
Lambdas used for sections should parse with the current delimiters.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Section - Alternate Delimiters',
  'desc' => 'Lambdas used for sections should parse with the current delimiters.',
  'data' => 
  array (
    'planet' => 'Earth',
    'lambda' => function ($text = '') { return $text . "{{planet}} => |planet|" . $text; },
  ),
  'template' => '{{= | | =}}<|#lambda|-|/lambda|>',
  'expected' => '<-{{planet}} => Earth->',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\<\-\{\{planet\}\}\s*\=\>\s*Earth\-\>