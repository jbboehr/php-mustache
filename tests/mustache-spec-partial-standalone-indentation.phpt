--TEST--
Standalone Indentation
--DESCRIPTION--
Each line of the partial should be indented before rendering.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Standalone Indentation',
  'desc' => 'Each line of the partial should be indented before rendering.',
  'data' => 
  array (
    'content' => '<
->',
  ),
  'template' => '\\
 {{>partial}}
/
',
  'partials' => 
  array (
    'partial' => '|
{{{content}}}
|
',
  ),
  'expected' => '\\
 |
 <
->
 |
/
',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
\\\s*\|\s*\<\s*\-\>\s*\|\s*\/\s*