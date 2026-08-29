--TEST--
Parent contexts
--DESCRIPTION--
Names missing in the current context are looked up in the stack.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Parent contexts',
  'desc' => 'Names missing in the current context are looked up in the stack.',
  'data' => 
  array (
    'a' => 'foo',
    'b' => 'wrong',
    'sec' => 
    array (
      'b' => 'bar',
    ),
    'c' => 
    array (
      'd' => 'baz',
    ),
  ),
  'template' => '"{{#sec}}{{a}}, {{b}}, {{c.d}}{{/sec}}"',
  'expected' => '"foo, bar, baz"',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>"foo, bar, baz"</render>
