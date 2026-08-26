--TEST--
Truthy
--DESCRIPTION--
Truthy sections should have their contents rendered.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Truthy',
  'desc' => 'Truthy sections should have their contents rendered.',
  'data' => 
  array (
    'boolean' => true,
  ),
  'template' => '"{{#boolean}}This should be rendered.{{/boolean}}"',
  'expected' => '"This should be rendered."',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>"This should be rendered."</render>