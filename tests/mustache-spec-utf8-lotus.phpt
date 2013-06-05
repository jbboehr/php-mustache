--TEST--
Lotus
--DESCRIPTION--
Lotus
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Lotus',
  'desc' => 'Lotus',
  'data' => 
  array (
  ),
  'template' => '妙法蓮華経',
  'expected' => '妙法蓮華経',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
妙法蓮華経