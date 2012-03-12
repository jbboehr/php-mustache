--TEST--
Ampersand Context Miss Interpolation
--DESCRIPTION--
Failed context lookups should default to empty strings.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Ampersand Context Miss Interpolation',
  'data' => 
  array (
  ),
  'expected' => 'I () be seen!',
  'template' => 'I ({{&cannot}}) be seen!',
  'desc' => 'Failed context lookups should default to empty strings.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
I\s*\(\)\s*be\s*seen\!