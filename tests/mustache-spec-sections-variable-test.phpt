--TEST--
Variable test
--DESCRIPTION--
Non-false sections have their value at the top of context,
accessible as {{.}} or through the parent context. This gives
a simple way to display content conditionally if a variable exists.

--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Variable test',
  'desc' => 'Non-false sections have their value at the top of context,
accessible as {{.}} or through the parent context. This gives
a simple way to display content conditionally if a variable exists.
',
  'data' => 
  array (
    'foo' => 'bar',
  ),
  'template' => '"{{#foo}}{{.}} is {{foo}}{{/foo}}"',
  'expected' => '"bar is bar"',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>"bar is bar"</render>
