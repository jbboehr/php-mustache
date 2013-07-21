--TEST--
Mustache::render() member function - Accepts MustacheAST
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = $m->parse('{{var}}');
var_dump(get_class($tmpl));
$r = $m->render($tmpl, array(
  'var' => 'val',
));
var_dump($r);
?>
--EXPECT--
string(11) "MustacheAST"
string(3) "val"