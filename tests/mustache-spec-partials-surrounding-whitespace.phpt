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
  'desc' => 'The greater-than operator should not alter surrounding whitespace.',
  'data' => 
  array (
  ),
  'template' => '| {{>partial}} |',
  'partials' => 
  array (
    'partial' => '	|	',
  ),
  'expected' => '| 	|	 |',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
\|\s*\|\s*\|