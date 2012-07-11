--TEST--
Indented Standalone Tag
--DESCRIPTION--
Indented standalone lines should be removed from the template.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Indented Standalone Tag',
  'desc' => 'Indented standalone lines should be removed from the template.',
  'data' => 
  array (
  ),
  'template' => 'Begin.
  {{=@ @=}}
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