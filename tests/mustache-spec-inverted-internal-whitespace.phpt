--TEST--
Internal Whitespace
--DESCRIPTION--
Inverted should not alter internal whitespace.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Internal Whitespace',
  'desc' => 'Inverted should not alter internal whitespace.',
  'data' => 
  array (
    'boolean' => false,
  ),
  'template' => ' | {{^boolean}} {{! Important Whitespace }}
 {{/boolean}} | 
',
  'expected' => ' |  
  | 
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s*\|\s*\|\s*