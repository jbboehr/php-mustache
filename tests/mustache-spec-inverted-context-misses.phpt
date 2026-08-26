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
  'desc' => 'Failed context lookups should be considered falsey.',
  'data' => 
  array (
  ),
  'template' => '[{{^missing}}Cannot find key \'missing\'!{{/missing}}]',
  'expected' => '[Cannot find key \'missing\'!]',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>[Cannot find key 'missing'!]</render>