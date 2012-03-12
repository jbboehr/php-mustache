--TEST--
Context Misses
--DESCRIPTION--
Failed context lookups should be considered falsey.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Context Misses',
  'data' => 
  array (
  ),
  'expected' => '[Cannot find key \'missing\'!]',
  'template' => '[{{^missing}}Cannot find key \'missing\'!{{/missing}}]',
  'desc' => 'Failed context lookups should be considered falsey.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\[Cannot\s*find\s*key\s*'missing'\!\]