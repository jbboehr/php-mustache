--TEST--
Triple Mustache Null Interpolation
--DESCRIPTION--
Nulls should interpolate as the empty string.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Triple Mustache Null Interpolation',
  'desc' => 'Nulls should interpolate as the empty string.',
  'data' => 
  array (
    'cannot' => NULL,
  ),
  'template' => 'I ({{{cannot}}}) be seen!',
  'expected' => 'I () be seen!',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>I () be seen!</render>
