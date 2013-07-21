--TEST--
Mustache::parse() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = $m->parse('{{test}}');
var_dump(get_class($tmpl));
?>
--EXPECT--
string(11) "MustacheAST"