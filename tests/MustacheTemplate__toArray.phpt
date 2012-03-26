--TEST--
MustacheTemplate::toArray() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = new MustacheTemplate('{{test}}');
$r = $m->compile($tmpl);
var_dump($r);
var_dump($tmpl->toArray());
?>
--EXPECT--
bool(true)
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