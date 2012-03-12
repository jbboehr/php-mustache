--TEST--
Indented Multiline Standalone
--DESCRIPTION--
All standalone comment lines should be removed.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Indented Multiline Standalone',
  'data' => 
  array (
  ),
  'expected' => 'Begin.
End.
',
  'template' => 'Begin.
  {{!
    Something\'s going on here...
  }}
End.
',
  'desc' => 'All standalone comment lines should be removed.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
Begin\.\s+End\.\s+