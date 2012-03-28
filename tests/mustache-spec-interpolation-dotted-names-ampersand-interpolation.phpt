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
  'data' => 
  array (
    'person' => 
    array (
      'name' => 'Joe',
    ),
  ),
  'expected' => '"Joe" == "Joe"',
  'template' => '"{{&person.name}}" == "{{#person}}{{&name}}{{/person}}"',
  'desc' => 'Dotted names should be considered a form of shorthand for sections.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"Joe"\s*\=\=\s*"Joe"