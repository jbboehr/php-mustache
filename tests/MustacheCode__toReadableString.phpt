--TEST--
MustacheCode::__toString() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$tmpl = new MustacheCode(pack("H*" , "000000020000000c00000016020000110001181008000300017465737400"));
var_dump($tmpl->toReadableString());
?>
--EXPECT--
string(287) "Symbol 000 @ 012
Symbol 001 @ 022
S012:      0x02 FUNCTION 0
F015: 000: 0x11 DLOOKUPSYM 1
F018: 001: 0x18 DPRINTE
F019: 002: 0x10 DPOP
F020: 003: 0x08 RETURN
N021:      0x00
S022:      0x03 STRING 1
C025: 000: 0x74 t
C026: 001: 0x65 e
C027: 002: 0x73 s
C028: 003: 0x74 t
N029:      0x00
"