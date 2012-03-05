--TEST--
Triple Mustache
--DESCRIPTION--
Triple mustaches should interpolate without HTML escaping.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Triple Mustache',
  'data' => 
  array (
    'forbidden' => '& " < >',
  ),
  'expected' => 'These characters should not be HTML escaped: & " < >
',
  'template' => 'These characters should not be HTML escaped: {{{forbidden}}}
',
  'desc' => 'Triple mustaches should interpolate without HTML escaping.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
These\s+characters\s+should\s+not\s+be\s+HTML\s+escaped\:\s+&\s+"\s+\<\s+\>\s+