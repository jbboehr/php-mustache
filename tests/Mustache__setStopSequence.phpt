--TEST--
Mustache::setStopSequence() member function
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$m->setStopSequence('%>');
echo $m->getStopSequence();
?>
--EXPECT--
%>