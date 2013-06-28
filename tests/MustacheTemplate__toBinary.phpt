--TEST--
MustacheTemplate::toBinary() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = new MustacheTemplate('{{test}}');
$r = $m->compile($tmpl);
var_dump($r);
var_dump(bin2hex($tmpl->toBinary()));
?>
--EXPECT--
bool(true)
string(66) "4d550001000000000001000000134d550010010000050000000000007465737400"