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
  'desc' => 'The equals sign (used on both sides) should permit delimiter changes.',
  'data' => 
  array (
    'text' => 'Hey!',
  ),
  'template' => '{{=<% %>=}}(<%text%>)',
  'expected' => '(Hey!)',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\(Hey\!\)