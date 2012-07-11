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
  'desc' => 'Unadorned tags should interpolate content into the template.',
  'data' => 
  array (
    'subject' => 'world',
  ),
  'template' => 'Hello, {{subject}}!
',
  'expected' => 'Hello, world!
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
Hello,\s*world\!\s*