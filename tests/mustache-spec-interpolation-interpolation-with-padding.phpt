--TEST--
Interpolation With Padding
--DESCRIPTION--
Superfluous in-tag whitespace should be ignored.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Interpolation With Padding',
  'desc' => 'Superfluous in-tag whitespace should be ignored.',
  'data' => 
  array (
    'string' => '---',
  ),
  'template' => '|{{ string }}|',
  'expected' => '|---|',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\-\-\-\|