--TEST--
Triple Mustache - Standalone
--DESCRIPTION--
Standalone interpolation should not alter surrounding whitespace.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Triple Mustache - Standalone',
  'desc' => 'Standalone interpolation should not alter surrounding whitespace.',
  'data' => 
  array (
    'string' => '---',
  ),
  'template' => '  {{{string}}}
',
  'expected' => '  ---
',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>  ---
</render>