--TEST--
Mustache::__construct() member function
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
echo get_class($m);
?>
--EXPECT--
Mustache