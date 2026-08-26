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
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>  12 
</render>