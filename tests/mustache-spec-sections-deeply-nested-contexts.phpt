--TEST--
Deeply Nested Contexts
--DESCRIPTION--
All elements on the context stack should be accessible.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Deeply Nested Contexts',
  'data' => 
  array (
    'a' => 
    array (
      'one' => 1,
    ),
    'b' => 
    array (
      'two' => 2,
    ),
    'c' => 
    array (
      'three' => 3,
    ),
    'd' => 
    array (
      'four' => 4,
    ),
    'e' => 
    array (
      'five' => 5,
    ),
  ),
  'expected' => '1
121
12321
1234321
123454321
1234321
12321
121
1
',
  'template' => '{{#a}}
{{one}}
{{#b}}
{{one}}{{two}}{{one}}
{{#c}}
{{one}}{{two}}{{three}}{{two}}{{one}}
{{#d}}
{{one}}{{two}}{{three}}{{four}}{{three}}{{two}}{{one}}
{{#e}}
{{one}}{{two}}{{three}}{{four}}{{five}}{{four}}{{three}}{{two}}{{one}}
{{/e}}
{{one}}{{two}}{{three}}{{four}}{{three}}{{two}}{{one}}
{{/d}}
{{one}}{{two}}{{three}}{{two}}{{one}}
{{/c}}
{{one}}{{two}}{{one}}
{{/b}}
{{one}}
{{/a}}
',
  'desc' => 'All elements on the context stack should be accessible.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
1\s*121\s*12321\s*1234321\s*123454321\s*1234321\s*12321\s*121\s*1\s*