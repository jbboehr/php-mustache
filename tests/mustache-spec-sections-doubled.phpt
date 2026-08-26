--TEST--
Doubled
--DESCRIPTION--
Multiple sections per template should be permitted.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Doubled',
  'desc' => 'Multiple sections per template should be permitted.',
  'data' => 
  array (
    'bool' => true,
    'two' => 'second',
  ),
  'template' => '{{#bool}}
* first
{{/bool}}
* {{two}}
{{#bool}}
* third
{{/bool}}
',
  'expected' => '* first
* second
* third
',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>* first
* second
* third
</render>