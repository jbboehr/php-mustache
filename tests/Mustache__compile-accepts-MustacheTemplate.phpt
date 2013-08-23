--TEST--
Mustache::compile() member function - Accepts MustacheTemplate
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = $m->compile(new MustacheTemplate('{{test}}'));
var_dump(get_class($tmpl));
var_dump(bin2hex((string) $tmpl));
?>
--EXPECT--
string(12) "MustacheCode"
string(60) "000000020000000c00000016020000110001181008000300017465737400"