--TEST--
Implicit Iterator - Decimal
--DESCRIPTION--
Implicit iterators should cast decimals to strings and interpolate.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Implicit Iterator - Decimal',
  'desc' => 'Implicit iterators should cast decimals to strings and interpolate.',
  'data' => 
  array (
    'list' => 
    array (
      0 => 1.1,
      1 => 2.2,
      2 => 3.3,
      3 => 4.4,
      4 => 5.5,
    ),
  ),
  'template' => '"{{#list}}({{.}}){{/list}}"',
  'expected' => '"(1.1)(2.2)(3.3)(4.4)(5.5)"',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"\(1\.1\)\(2\.2\)\(3\.3\)\(4\.4\)\(5\.5\)"