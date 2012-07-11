--TEST--
Pair with Padding
--DESCRIPTION--
Superfluous in-tag whitespace should be ignored.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Pair with Padding',
  'desc' => 'Superfluous in-tag whitespace should be ignored.',
  'data' => 
  array (
  ),
  'template' => '|{{= @   @ =}}|',
  'expected' => '||',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\|