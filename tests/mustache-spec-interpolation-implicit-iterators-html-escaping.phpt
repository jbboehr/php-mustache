--TEST--
Implicit Iterators - HTML Escaping
--DESCRIPTION--
Basic interpolation should be HTML escaped.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Implicit Iterators - HTML Escaping',
  'desc' => 'Basic interpolation should be HTML escaped.',
  'data' => '& " < >',
  'template' => 'These characters should be HTML escaped: {{.}}
',
  'expected' => 'These characters should be HTML escaped: &amp; &quot; &lt; &gt;
',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>These characters should be HTML escaped: &amp; &quot; &lt; &gt;
</render>
