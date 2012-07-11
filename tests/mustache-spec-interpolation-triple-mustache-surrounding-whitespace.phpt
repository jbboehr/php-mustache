--TEST--
Triple Mustache - Surrounding Whitespace
--DESCRIPTION--
Interpolation should not alter surrounding whitespace.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Triple Mustache - Surrounding Whitespace',
  'desc' => 'Interpolation should not alter surrounding whitespace.',
  'data' => 
  array (
    'string' => '---',
  ),
  'template' => '| {{{string}}} |',
  'expected' => '| --- |',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\s*\-\-\-\s*\|