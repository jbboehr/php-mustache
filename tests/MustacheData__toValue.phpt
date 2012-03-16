--TEST--
MustacheData::toValue() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$data = new MustacheData(array('a' => 'b'));
var_dump($data->toValue());
?>
--EXPECT--
array(1) {
  ["a"]=>
  string(1) "b"
}