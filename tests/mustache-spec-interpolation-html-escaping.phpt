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
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>These characters should be HTML escaped: &amp; &quot; &lt; &gt;
</render>