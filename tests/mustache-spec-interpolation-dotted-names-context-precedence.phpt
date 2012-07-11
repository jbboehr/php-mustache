--TEST--
Dotted Names - Context Precedence
--DESCRIPTION--
Dotted names should be resolved against former resolutions.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Dotted Names - Context Precedence',
  'desc' => 'Dotted names should be resolved against former resolutions.',
  'data' => 
  array (
    'a' => 
    array (
      'b' => 
      array (
      ),
    ),
    'b' => 
    array (
      'c' => 'ERROR',
    ),
  ),
  'template' => '{{#a}}{{b.c}}{{/a}}',
  'expected' => '',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
