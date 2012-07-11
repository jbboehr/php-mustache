--TEST--
Standalone Without Newline
--DESCRIPTION--
Standalone tags should not require a newline to follow them.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Standalone Without Newline',
  'desc' => 'Standalone tags should not require a newline to follow them.',
  'data' => 
  array (
  ),
  'template' => '=
  {{=@ @=}}',
  'expected' => '=
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\=\s*