--TEST--
Basic Interpolation
--DESCRIPTION--
Unadorned tags should interpolate content into the template.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Basic Interpolation',
  'data' => 
  array (
    'subject' => 'world',
  ),
  'expected' => 'Hello, world!
',
  'template' => 'Hello, {{subject}}!
',
  'desc' => 'Unadorned tags should interpolate content into the template.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
Hello,\s+world\!\s+