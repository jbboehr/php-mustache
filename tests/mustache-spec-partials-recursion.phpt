--TEST--
Recursion
--DESCRIPTION--
The greater-than operator should properly recurse.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Recursion',
  'data' => 
  array (
    'content' => 'X',
    'nodes' => 
    array (
      0 => 
      array (
        'content' => 'Y',
        'nodes' => 
        array (
        ),
      ),
    ),
  ),
  'expected' => 'X<Y<>>',
  'template' => '{{>node}}',
  'desc' => 'The greater-than operator should properly recurse.',
  'partials' => 
  array (
    'node' => '{{content}}<{{#nodes}}{{>node}}{{/nodes}}>',
  ),
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
X\<Y\<\>\>