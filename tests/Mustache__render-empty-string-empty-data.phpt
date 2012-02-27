--TEST--
Mustache::render() member function - empty string and data
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$r = $m->render('', array());
var_dump($r);
?>
--EXPECT--
string(0) ""