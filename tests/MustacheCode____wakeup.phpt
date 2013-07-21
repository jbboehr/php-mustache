--TEST--
MustacheCode::__wakeup() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$tmpl = new MustacheCode(pack("H*" , "000000020000000c00000014020022012921160003017465737400"));
$serial = unserialize(serialize($tmpl));
var_dump(bin2hex($serial->binaryString));
var_dump(bin2hex((string) $serial));
?>
--EXPECT--
string(54) "000000020000000c00000014020022012921160003017465737400"
string(54) "000000020000000c00000014020022012921160003017465737400"