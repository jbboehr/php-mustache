--TEST--
Triple Mustache Context Miss Interpolation
--DESCRIPTION--
Failed context lookups should default to empty strings.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Triple Mustache Context Miss Interpolation',
  'desc' => 'Failed context lookups should default to empty strings.',
  'data' => 
  array (
  ),
  'template' => 'I ({{{cannot}}}) be seen!',
  'expected' => 'I () be seen!',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
I\s*\(\)\s*be\s*seen\!