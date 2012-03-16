--TEST--
Mustache::debugDataStructure() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
var_dump($m->debugDataStructure(array('a' => 'b')));
?>
--EXPECT--
array(1) {
  ["a"]=>
  string(1) "b"
}