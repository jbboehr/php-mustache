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
  'data' => 
  array (
    'boolean' => true,
  ),
  'expected' => '|[]|',
  'template' => '|{{> partial }}|',
  'desc' => 'Superfluous in-tag whitespace should be ignored.',
  'partials' => 
  array (
    'partial' => '[]',
  ),
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
\|\[\]\|