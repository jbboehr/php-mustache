--TEST--
Standalone Indented Lines
--DESCRIPTION--
Standalone indented lines should be removed from the template.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Standalone Indented Lines',
  'data' => 
  array (
    'boolean' => false,
  ),
  'expected' => '| This Is
|
| A Line
',
  'template' => '| This Is
  {{^boolean}}
|
  {{/boolean}}
| A Line
',
  'desc' => 'Standalone indented lines should be removed from the template.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\s*This\s*Is\s*\|\s*\|\s*A\s*Line\s*