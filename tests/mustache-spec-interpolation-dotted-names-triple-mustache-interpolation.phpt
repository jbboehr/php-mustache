--TEST--
Dotted Names - Triple Mustache Interpolation
--DESCRIPTION--
Dotted names should be considered a form of shorthand for sections.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Dotted Names - Triple Mustache Interpolation',
  'desc' => 'Dotted names should be considered a form of shorthand for sections.',
  'data' => 
  array (
    'person' => 
    array (
      'name' => 'Joe',
    ),
  ),
  'template' => '"{{{person.name}}}" == "{{#person}}{{{name}}}{{/person}}"',
  'expected' => '"Joe" == "Joe"',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>"Joe" == "Joe"</render>