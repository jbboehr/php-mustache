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
  'data' => 
  array (
    'boolean' => false,
  ),
  'expected' => '|
|',
  'template' => '|
{{^boolean}}
{{/boolean}}
|',
  'desc' => '"\\r\\n" should be considered a newline for standalone tags.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\s*\|