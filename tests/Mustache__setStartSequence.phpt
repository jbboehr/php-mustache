--TEST--
Mustache::setStartSequence() member function
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$m->setStartSequence('<%');
echo $m->getStartSequence();
?>
--EXPECT--
<%