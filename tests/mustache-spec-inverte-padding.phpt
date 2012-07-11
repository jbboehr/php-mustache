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
  'desc' => 'Superfluous in-tag whitespace should be ignored.',
  'data' => 
  array (
    'boolean' => false,
  ),
  'template' => '|{{^ boolean }}={{/ boolean }}|',
  'expected' => '|=|',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\=\|