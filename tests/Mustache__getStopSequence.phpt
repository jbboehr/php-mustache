--TEST--
Mustache::getStopSequence() member function
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
echo $m->getStopSequence();
?>
--EXPECT--
}}