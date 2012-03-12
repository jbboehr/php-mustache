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
  'data' => 
  array (
    'data' => '|',
  ),
  'expected' => '  |  >
>
',
  'template' => '  {{data}}  {{> partial}}
',
  'desc' => 'Whitespace should be left untouched.',
  'partials' => 
  array (
    'partial' => '>
>',
  ),
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\s*\|\s*\>\s*\>\s*