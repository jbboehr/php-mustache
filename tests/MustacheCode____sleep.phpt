--TEST--
MustacheCode::__sleep() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$tmpl = new MustacheCode(pack("H*" , "000000020000000c00000014020011011810080003017465737400"));
$serial = serialize($tmpl);
var_dump(addslashes($serial));
?>
--EXPECT--
string(97) "O:12:\"MustacheCode\":1:{s:12:\"binaryString\";s:27:\"\0\0\0\0\0\0\0\0\0\0\0test\0\";}"