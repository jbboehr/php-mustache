--TEST--
Mustache::getEscapeByDefault() member function
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
var_dump($m->getEscapeByDefault());
?>
--EXPECT--
bool(true)