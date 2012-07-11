--TEST--
Standalone Lines
--DESCRIPTION--
Standalone lines should be removed from the template.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Standalone Lines',
  'desc' => 'Standalone lines should be removed from the template.',
  'data' => 
  array (
    'boolean' => true,
  ),
  'template' => '| This Is
{{#boolean}}
|
{{/boolean}}
| A Line
',
  'expected' => '| This Is
|
| A Line
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\s*This\s*Is\s*\|\s*\|\s*A\s*Line\s*