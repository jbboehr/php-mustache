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
  'data' => 
  array (
    'bool' => false,
  ),
  'expected' => '| A  E |',
  'template' => '| A {{#bool}}B {{#bool}}C{{/bool}} D{{/bool}} E |',
  'desc' => 'Nested falsey sections should be omitted.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\s*A\s*E\s*\|