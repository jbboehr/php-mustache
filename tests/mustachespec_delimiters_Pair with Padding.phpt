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
  'data' => 
  array (
  ),
  'expected' => '||',
  'template' => '|{{= @   @ =}}|',
  'desc' => 'Superfluous in-tag whitespace should be ignored.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\|