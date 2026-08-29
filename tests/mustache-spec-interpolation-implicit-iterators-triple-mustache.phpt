--TEST--
Implicit Iterators - Triple Mustache
--DESCRIPTION--
Triple mustaches should interpolate without HTML escaping.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Implicit Iterators - Triple Mustache',
  'desc' => 'Triple mustaches should interpolate without HTML escaping.',
  'data' => '& " < >',
  'template' => 'These characters should not be HTML escaped: {{{.}}}
',
  'expected' => 'These characters should not be HTML escaped: & " < >
',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>These characters should not be HTML escaped: & " < >
</render>
