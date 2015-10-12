--TEST--
Escaping
--DESCRIPTION--
Lambda results should be appropriately escaped.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Escaping',
  'desc' => 'Lambda results should be appropriately escaped.',
  'data' => 
  array (
    'lambda' => function ($text = '') { return ">"; },
  ),
  'template' => '<{{lambda}}{{{lambda}}}',
  'expected' => '<&gt;>',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\<&gt;\>