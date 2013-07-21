--TEST--
MustacheCode::__wakeup() member function with APC
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
if( !extension_loaded('apc') ) {
  die('skip ' . "try running with: make test TEST_PHP_ARGS='-d extension=/usr/lib/php5/20090626/apc.so -d apc.enable_cli=1'");
}
?>
--FILE--
<?php
ini_set('apc.enable_cli', 1);
$tmpl = new MustacheCode(pack("H*" , "000000020000000c00000014020022012921160003017465737400"));
$ret = apc_store(md5(__FILE__), serialize($tmpl));
$tmpl = unserialize(apc_fetch(md5(__FILE__)));
var_dump(bin2hex($tmpl->binaryString));
var_dump(bin2hex((string) $tmpl));
?>
--EXPECT--
string(54) "000000020000000c00000014020022012921160003017465737400"
string(54) "000000020000000c00000014020022012921160003017465737400"