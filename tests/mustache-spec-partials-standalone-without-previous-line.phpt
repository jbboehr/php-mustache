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
  'data' => 
  array (
  ),
  'expected' => '  >
  >>',
  'template' => '  {{>partial}}
>',
  'desc' => 'Standalone tags should not require a newline to precede them.',
  'partials' => 
  array (
    'partial' => '>
>',
  ),
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
\s*>\s*>\s*>\s*