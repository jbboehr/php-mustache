--TEST--
MustacheTemplate::__construct() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$tmpl = new MustacheTemplate('{{test}}');
var_dump(get_class($tmpl));
?>
--EXPECT--
string(16) "MustacheTemplate"