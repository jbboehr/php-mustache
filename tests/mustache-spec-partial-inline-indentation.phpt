--TEST--
Inline Indentation
--DESCRIPTION--
Whitespace should be left untouched.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Inline Indentation',
  'desc' => 'Whitespace should be left untouched.',
  'data' => 
  array (
    'data' => '|',
  ),
  'template' => '  {{data}}  {{> partial}}
',
  'partials' => 
  array (
    'partial' => '>
>',
  ),
  'expected' => '  |  >
>
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
\s*\|\s*\>\s*\>\s*