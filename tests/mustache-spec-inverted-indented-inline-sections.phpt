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
  'data' => 
  array (
    'boolean' => false,
  ),
  'expected' => ' NO
 WAY
',
  'template' => ' {{^boolean}}NO{{/boolean}}
 {{^boolean}}WAY{{/boolean}}
',
  'desc' => 'Single-line sections should not alter surrounding whitespace.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s*NO\s*WAY\s*