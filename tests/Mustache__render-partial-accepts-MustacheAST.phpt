--TEST--
Mustache::render() member function - Accepts MustacheAST partials
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = '{{>test1}}{{>test2}}';
$data = new MustacheData(array(
  'var' => 'val',
));
$ast = $m->parse('{{var}}');
$astBinary = (string) $ast;
$partials = array(
  'test1' => '{{var}}',
  'test2' => $ast,
);
$r = $m->render($tmpl, $data, $partials);
var_dump($r);
var_dump((string) $ast === $astBinary);

$astTemplate = $m->parse('{{>test}}');
var_dump($m->render($astTemplate, $data, array('test' => '{{var}}')));
?>
--EXPECT--
string(6) "valval"
bool(true)
string(3) "val"
