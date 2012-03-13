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
  ),
  'expected' => '|
>|',
  'template' => '|
{{>partial}}
|',
  'desc' => '"\\r\\n" should be considered a newline for standalone tags.',
  'partials' => 
  array (
    'partial' => '>',
  ),
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
\|\s*\>\|
--XFAIL--
This extension does not follow the spec's whitespace rules.