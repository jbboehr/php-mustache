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
  'data' => 
  array (
    'content' => '<
->',
  ),
  'expected' => '\\
 |
 <
->
 |
/
',
  'template' => '\\
 {{>partial}}
/
',
  'desc' => 'Each line of the partial should be indented before rendering.',
  'partials' => 
  array (
    'partial' => '|
{{{content}}}
|
',
  ),
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
\\\s*\|\s*\<\s*\-\>\s*\|\s*\/\s*