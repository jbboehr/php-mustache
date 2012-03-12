--TEST--
Surrounding Whitespace
--DESCRIPTION--
The greater-than operator should not alter surrounding whitespace.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Surrounding Whitespace',
  'data' => 
  array (
  ),
  'expected' => '| 	|	 |',
  'template' => '| {{>partial}} |',
  'desc' => 'The greater-than operator should not alter surrounding whitespace.',
  'partials' => 
  array (
    'partial' => '	|	',
  ),
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\s*\|\s*\|