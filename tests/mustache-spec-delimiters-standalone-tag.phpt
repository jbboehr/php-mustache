--TEST--
Standalone Tag
--DESCRIPTION--
Standalone lines should be removed from the template.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Standalone Tag',
  'desc' => 'Standalone lines should be removed from the template.',
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
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>Begin.
End.
</render>