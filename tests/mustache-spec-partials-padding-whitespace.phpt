--TEST--
Padding Whitespace
--DESCRIPTION--
Superfluous in-tag whitespace should be ignored.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Padding Whitespace',
  'desc' => 'Superfluous in-tag whitespace should be ignored.',
  'data' => 
  array (
    'boolean' => true,
  ),
  'template' => '|{{> partial }}|',
  'partials' => 
  array (
    'partial' => '[]',
  ),
  'expected' => '|[]|',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
\|\[\]\|