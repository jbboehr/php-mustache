--TEST--
Mustache::render() member function - Accepts MustacheAST
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = $m->compile('{{var}}');
$r = $m->render($tmpl, array(
  'var' => 'val',
));
var_dump($r);
?>
--EXPECT--
string(3) "val"