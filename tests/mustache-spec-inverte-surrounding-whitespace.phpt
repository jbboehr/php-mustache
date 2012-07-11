--TEST--
Surrounding Whitespace
--DESCRIPTION--
Inverted sections should not alter surrounding whitespace.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Surrounding Whitespace',
  'desc' => 'Inverted sections should not alter surrounding whitespace.',
  'data' => 
  array (
    'boolean' => false,
  ),
  'template' => ' | {{^boolean}}	|	{{/boolean}} | 
',
  'expected' => ' | 	|	 | 
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s*\|\s*\|\s*\|\s*