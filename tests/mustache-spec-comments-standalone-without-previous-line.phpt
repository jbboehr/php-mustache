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
  'expected' => '!',
  'template' => '  {{! I\'m Still Standalone }}
!',
  'desc' => 'Standalone tags should not require a newline to precede them.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\!