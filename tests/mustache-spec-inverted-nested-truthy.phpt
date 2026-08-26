--TEST--
Nested (Truthy)
--DESCRIPTION--
Nested truthy sections should be omitted.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Nested (Truthy)',
  'desc' => 'Nested truthy sections should be omitted.',
  'data' => 
  array (
    'bool' => true,
  ),
  'template' => '| A {{^bool}}B {{^bool}}C{{/bool}} D{{/bool}} E |',
  'expected' => '| A  E |',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>| A  E |</render>