--TEST--
Padding
--DESCRIPTION--
Superfluous in-tag whitespace should be ignored.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Padding',
  'data' => 
  array (
    'boolean' => true,
  ),
  'expected' => '|=|',
  'template' => '|{{# boolean }}={{/ boolean }}|',
  'desc' => 'Superfluous in-tag whitespace should be ignored.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\=\|