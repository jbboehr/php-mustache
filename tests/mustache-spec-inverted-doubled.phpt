--TEST--
Doubled
--DESCRIPTION--
Multiple inverted sections per template should be permitted.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Doubled',
  'data' => 
  array (
    'two' => 'second',
    'bool' => false,
  ),
  'expected' => '* first
* second
* third
',
  'template' => '{{^bool}}
* first
{{/bool}}
* {{two}}
{{^bool}}
* third
{{/bool}}
',
  'desc' => 'Multiple inverted sections per template should be permitted.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\*\s*first\s*\*\s*second\s*\*\s*third\s*