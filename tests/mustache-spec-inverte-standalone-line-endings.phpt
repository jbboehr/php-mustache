--TEST--
Standalone Line Endings
--DESCRIPTION--
"\r\n" should be considered a newline for standalone tags.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Standalone Line Endings',
  'desc' => '"\\r\\n" should be considered a newline for standalone tags.',
  'data' => 
  array (
    'boolean' => false,
  ),
  'template' => '|
{{^boolean}}
{{/boolean}}
|',
  'expected' => '|
|',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\s*\|