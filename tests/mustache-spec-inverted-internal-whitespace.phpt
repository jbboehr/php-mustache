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
  'data' => 
  array (
    'boolean' => false,
  ),
  'expected' => ' |  
  | 
',
  'template' => ' | {{^boolean}} {{! Important Whitespace }}
 {{/boolean}} | 
',
  'desc' => 'Inverted should not alter internal whitespace.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s*\|\s*\|\s*