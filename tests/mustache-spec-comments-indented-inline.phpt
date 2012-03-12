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
  'data' => 
  array (
  ),
  'expected' => '  12 
',
  'template' => '  12 {{! 34 }}
',
  'desc' => 'Inline comments should not strip whitespace',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s*12\s*