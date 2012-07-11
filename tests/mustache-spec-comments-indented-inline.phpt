--TEST--
Indented Inline
--DESCRIPTION--
Inline comments should not strip whitespace
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Indented Inline',
  'desc' => 'Inline comments should not strip whitespace',
  'data' => 
  array (
  ),
  'template' => '  12 {{! 34 }}
',
  'expected' => '  12 
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s*12\s*