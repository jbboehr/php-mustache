--TEST--
MustacheTemplate::toArray() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$tmpl = new MustacheTemplate('{{test}}');
var_dump($tmpl->toArray());
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
      int(3)
      ["flags"]=>
      int(0)
      ["data"]=>
      string(4) "test"
    }
  }
}