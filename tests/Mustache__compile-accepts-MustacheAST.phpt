--TEST--
Mustache::compile() member function - Accepts MustacheAST
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$ast = $m->parse('{{test}}');
$tmpl = $m->compile($ast);
var_dump(get_class($tmpl));
var_dump(bin2hex((string) $tmpl));
?>
--EXPECT--
string(12) "MustacheCode"
string(54) "000000020000000c00000014020022012921160003017465737400"