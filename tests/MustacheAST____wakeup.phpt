--TEST--
MustacheAST::__wakeup() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
if( !function_exists('hex2bin') ) die('skip needs hex2bin');
?>
--FILE--
<?php
$tmpl = new MustacheAST(hex2bin("4d550001000000000001000000134d550010010000050000000000007465737400"));
$tmpl = unserialize(serialize($tmpl));
var_dump($tmpl instanceof MustacheAST);
$r = new ReflectionProperty($tmpl, 'binaryString');
if (PHP_VERSION_ID < 80100) {
  $r->setAccessible(true);
}
var_dump(bin2hex($r->getValue($tmpl)));
var_dump(bin2hex($tmpl->__toString()));
$m = new Mustache();
var_dump($m->render($tmpl, array('test' => 'baz')));
?>
--EXPECT--
bool(true)
string(66) "4d550001000000000001000000134d550010010000050000000000007465737400"
string(66) "4d550001000000000001000000134d550010010000050000000000007465737400"
string(3) "baz"
