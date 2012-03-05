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
  'data' => 
  array (
    'boolean' => false,
  ),
  'expected' => ' | 	|	 | 
',
  'template' => ' | {{^boolean}}	|	{{/boolean}} | 
',
  'desc' => 'Inverted sections should not alter surrounding whitespace.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s+\|\s+\|\s+\|\s+