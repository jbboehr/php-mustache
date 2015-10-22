--TEST--
Interpolation - Alternate Delimiters
--DESCRIPTION--
A lambda's return value should parse with the default delimiters.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Interpolation - Alternate Delimiters',
  'desc' => 'A lambda\'s return value should parse with the default delimiters.',
  'data' => 
  array (
    'planet' => 'world',
    'lambda' => function ($text = '') { return "|planet| => {{planet}}"; },
  ),
  'template' => '{{= | | =}}
Hello, (|&lambda|)!',
  'expected' => 'Hello, (|planet| => world)!',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
Hello,\s*\(\|planet\|\s*\=\>\s*world\)\!