--TEST--
Pair Behavior
--DESCRIPTION--
The equals sign (used on both sides) should permit delimiter changes.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Pair Behavior',
  'data' => 
  array (
    'text' => 'Hey!',
  ),
  'expected' => '(Hey!)',
  'template' => '{{=<% %>=}}(<%text%>)',
  'desc' => 'The equals sign (used on both sides) should permit delimiter changes.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\(Hey\!\)