--TEST--
Ampersand
--DESCRIPTION--
Ampersand should interpolate without HTML escaping.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Ampersand',
  'data' => 
  array (
    'forbidden' => '& " < >',
  ),
  'expected' => 'These characters should not be HTML escaped: & " < >
',
  'template' => 'These characters should not be HTML escaped: {{&forbidden}}
',
  'desc' => 'Ampersand should interpolate without HTML escaping.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
These\s+characters\s+should\s+not\s+be\s+HTML\s+escaped\:\s+&\s+"\s+\<\s+\>\s+