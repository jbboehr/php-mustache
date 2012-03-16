--TEST--
MustacheTemplate::__toString() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$tmpl = new MustacheTemplate('{{test}}');
var_dump($tmpl->__toString());
?>
--EXPECT--
string(8) "{{test}}"