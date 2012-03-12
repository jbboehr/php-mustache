--TEST--
Mustache::setEscapeByDefault() member function
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$m->setEscapeByDefault(false);
var_dump($m->getEscapeByDefault());
?>
--EXPECT--
bool(false)