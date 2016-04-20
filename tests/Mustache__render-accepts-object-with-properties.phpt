--TEST--
Mustache::render() member function - Accepts object with predefined properties (zval for property will be IS_INDIRECT)
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
class Data {
  public $var = 'val';
}
$m = new Mustache();
$data = new Data();
$r = $m->render('{{var}}', $data);
var_dump($r);
?>
--EXPECT--
string(3) "val"
