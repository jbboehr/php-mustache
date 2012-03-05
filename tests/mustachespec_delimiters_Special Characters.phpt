--TEST--
Special Characters
--DESCRIPTION--
Characters with special meaning regexen should be valid delimiters.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Special Characters',
  'data' => 
  array (
    'text' => 'It worked!',
  ),
  'expected' => '(It worked!)',
  'template' => '({{=[ ]=}}[text])',
  'desc' => 'Characters with special meaning regexen should be valid delimiters.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\(It\s+worked\!\)