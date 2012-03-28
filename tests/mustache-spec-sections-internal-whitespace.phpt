--TEST--
Internal Whitespace
--DESCRIPTION--
Sections should not alter internal whitespace.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Internal Whitespace',
  'data' => 
  array (
    'boolean' => true,
  ),
  'expected' => ' |  
  | 
',
  'template' => ' | {{#boolean}} {{! Important Whitespace }}
 {{/boolean}} | 
',
  'desc' => 'Sections should not alter internal whitespace.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s*\|\s*\|\s*