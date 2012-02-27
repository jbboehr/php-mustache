--TEST--
Mustache::render() member function - empty string with data
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$r = $m->render('', array(
  'test' => 'test',
  'test2' => array(
      'a' => 'b',
  ),
));
var_dump($r);
?>
--EXPECT--
string(0) ""