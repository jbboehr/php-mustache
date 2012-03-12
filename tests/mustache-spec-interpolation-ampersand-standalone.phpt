--TEST--
Ampersand - Standalone
--DESCRIPTION--
Standalone interpolation should not alter surrounding whitespace.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Ampersand - Standalone',
  'data' => 
  array (
    'string' => '---',
  ),
  'expected' => '  ---
',
  'template' => '  {{&string}}
',
  'desc' => 'Standalone interpolation should not alter surrounding whitespace.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s+\-\-\-\s+