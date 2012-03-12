--TEST--
Mustache::getStartSequence() member function
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
echo $m->getStartSequence();
?>
--EXPECT--
{{