--TEST--
MustacheCode::__toString() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$tmpl = new MustacheCode(pack("H*" , "000000020000000c00000014020011011810080003017465737400"));
var_dump(bin2hex($tmpl->__toString()));
var_dump(bin2hex((string) $tmpl));
?>
--EXPECT--
string(54) "000000020000000c00000014020011011810080003017465737400"
string(54) "000000020000000c00000014020011011810080003017465737400"