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
  ),
  'template' => '|
{{>partial}}
|',
  'partials' => 
  array (
    'partial' => '>',
  ),
  'expected' => '|
>|',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
\s*|\s*>\s*|\s*