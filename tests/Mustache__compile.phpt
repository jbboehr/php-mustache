--TEST--
Mustache::compile() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = $m->compile('{{test}}');
var_dump(get_class($tmpl));
?>
--EXPECT--
string(16) "MustacheTemplate"