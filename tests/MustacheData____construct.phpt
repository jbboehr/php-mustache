--TEST--
MustacheData::__construct() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$data = new MustacheData(array('a' => 'b'));
var_dump(get_class($data));
?>
--EXPECT--
string(12) "MustacheData"