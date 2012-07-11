--TEST--
Standalone Without Previous Line
--DESCRIPTION--
Standalone tags should not require a newline to precede them.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Standalone Without Previous Line',
  'desc' => 'Standalone tags should not require a newline to precede them.',
  'data' => 
  array (
    'boolean' => false,
  ),
  'template' => '  {{^boolean}}
^{{/boolean}}
/',
  'expected' => '^
/',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\^\s*\/