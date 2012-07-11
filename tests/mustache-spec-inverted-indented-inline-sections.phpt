--TEST--
Indented Inline Sections
--DESCRIPTION--
Single-line sections should not alter surrounding whitespace.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Indented Inline Sections',
  'desc' => 'Single-line sections should not alter surrounding whitespace.',
  'data' => 
  array (
    'boolean' => false,
  ),
  'template' => ' {{^boolean}}NO{{/boolean}}
 {{^boolean}}WAY{{/boolean}}
',
  'expected' => ' NO
 WAY
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s*NO\s*WAY\s*