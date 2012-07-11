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
  'desc' => 'All standalone comment lines should be removed.',
  'data' => 
  array (
  ),
  'template' => 'Begin.
  {{!
    Something\'s going on here...
  }}
End.
',
  'expected' => 'Begin.
End.
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
Begin\.\s*End\.\s*