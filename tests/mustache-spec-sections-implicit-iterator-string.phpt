--TEST--
Implicit Iterator - String
--DESCRIPTION--
Implicit iterators should directly interpolate strings.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Implicit Iterator - String',
  'desc' => 'Implicit iterators should directly interpolate strings.',
  'data' => 
  array (
    'list' => 
    array (
      0 => 'a',
      1 => 'b',
      2 => 'c',
      3 => 'd',
      4 => 'e',
    ),
  ),
  'template' => '"{{#list}}({{.}}){{/list}}"',
  'expected' => '"(a)(b)(c)(d)(e)"',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"\(a\)\(b\)\(c\)\(d\)\(e\)"