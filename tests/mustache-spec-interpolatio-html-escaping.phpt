--TEST--
HTML Escaping
--DESCRIPTION--
Basic interpolation should be HTML escaped.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'HTML Escaping',
  'desc' => 'Basic interpolation should be HTML escaped.',
  'data' => 
  array (
    'forbidden' => '& " < >',
  ),
  'template' => 'These characters should be HTML escaped: {{forbidden}}
',
  'expected' => 'These characters should be HTML escaped: &amp; &quot; &lt; &gt;
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
These\s*characters\s*should\s*be\s*HTML\s*escaped\:\s*&amp;\s*&quot;\s*&lt;\s*&gt;\s*