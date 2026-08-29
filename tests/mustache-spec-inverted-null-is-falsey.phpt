--TEST--
Null is falsey
--DESCRIPTION--
Null is falsey.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Null is falsey',
  'desc' => 'Null is falsey.',
  'data' => 
  array (
    'null' => NULL,
  ),
  'template' => '"{{^null}}This should be rendered.{{/null}}"',
  'expected' => '"This should be rendered."',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>"This should be rendered."</render>
