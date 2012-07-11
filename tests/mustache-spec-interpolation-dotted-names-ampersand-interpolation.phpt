--TEST--
Dotted Names - Ampersand Interpolation
--DESCRIPTION--
Dotted names should be considered a form of shorthand for sections.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Dotted Names - Ampersand Interpolation',
  'desc' => 'Dotted names should be considered a form of shorthand for sections.',
  'data' => 
  array (
    'person' => 
    array (
      'name' => 'Joe',
    ),
  ),
  'template' => '"{{&person.name}}" == "{{#person}}{{&name}}{{/person}}"',
  'expected' => '"Joe" == "Joe"',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"Joe"\s*\=\=\s*"Joe"