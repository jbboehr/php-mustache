--TEST--
Nested (Falsey)
--DESCRIPTION--
Nested falsey sections should have their contents rendered.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Nested (Falsey)',
  'desc' => 'Nested falsey sections should have their contents rendered.',
  'data' => 
  array (
    'bool' => false,
  ),
  'template' => '| A {{^bool}}B {{^bool}}C{{/bool}} D{{/bool}} E |',
  'expected' => '| A B C D E |',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\|\s*A\s*B\s*C\s*D\s*E\s*\|