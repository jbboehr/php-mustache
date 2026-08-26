--TEST--
Nested (Falsey)
--DESCRIPTION--
Nested falsey sections should be omitted.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Nested (Falsey)',
  'desc' => 'Nested falsey sections should be omitted.',
  'data' => 
  array (
    'bool' => false,
  ),
  'template' => '| A {{#bool}}B {{#bool}}C{{/bool}} D{{/bool}} E |',
  'expected' => '| A  E |',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>| A  E |</render>