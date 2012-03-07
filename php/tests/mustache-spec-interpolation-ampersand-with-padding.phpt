--TEST--
Ampersand With Padding
--DESCRIPTION--
Superfluous in-tag whitespace should be ignored.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Ampersand With Padding',
  'data' => 
  array (
    'string' => '---',
  ),
  'expected' => '|---|',
  'template' => '|{{& string }}|',
  'desc' => 'Superfluous in-tag whitespace should be ignored.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\-\-\-\|