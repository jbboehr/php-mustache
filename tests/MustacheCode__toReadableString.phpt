--TEST--
MustacheCode::__toString() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$tmpl = new MustacheCode(pack("H*" , "000000020000000c00000014020011011810080003017465737400"));
var_dump($tmpl->toReadableString());
?>
--EXPECT--
string(287) "Symbol 000 @ 012
Symbol 001 @ 020
S012:      0x02 FUNCTION 0
F014: 000: 0x11 DLOOKUPSYM 1
F016: 001: 0x18 DPRINTE
F017: 002: 0x10 DPOP
F018: 003: 0x08 RETURN
N019:      0x00
S020:      0x03 STRING 1
C022: 000: 0x74 t
C023: 001: 0x65 e
C024: 002: 0x73 s
C025: 003: 0x74 t
N026:      0x00
"