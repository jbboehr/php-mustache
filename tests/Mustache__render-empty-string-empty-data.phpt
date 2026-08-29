--TEST--
Mustache::render() member function - empty string and data with omitted or null partials
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$r = $m->render('', array());
var_dump($r);
$r = $m->render('', array(), null);
var_dump($r);
?>
--EXPECT--
string(0) ""
string(0) ""
