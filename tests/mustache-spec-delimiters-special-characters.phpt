--TEST--
Special Characters
--DESCRIPTION--
Characters with special meaning regexen should be valid delimiters.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Special Characters',
  'desc' => 'Characters with special meaning regexen should be valid delimiters.',
  'data' => 
  array (
    'text' => 'It worked!',
  ),
  'template' => '({{=[ ]=}}[text])',
  'expected' => '(It worked!)',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>(It worked!)</render>