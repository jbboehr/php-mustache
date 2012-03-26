--TEST--
Mustache::tokenize() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
var_dump($m->tokenize('{{test}}'));
?>
--EXPECT--
array(3) {
  ["type"]=>
  int(1)
  ["flags"]=>
  int(0)
  ["children"]=>
  array(1) {
    [0]=>
    array(3) {
      ["type"]=>
      int(16)
      ["flags"]=>
      int(1)
      ["data"]=>
      string(4) "test"
    }
  }
}