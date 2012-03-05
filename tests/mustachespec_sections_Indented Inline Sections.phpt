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
    'boolean' => true,
  ),
  'expected' => ' YES
 GOOD
',
  'template' => ' {{#boolean}}YES{{/boolean}}
 {{#boolean}}GOOD{{/boolean}}
',
  'desc' => 'Single-line sections should not alter surrounding whitespace.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s+YES\s+GOOD\s+