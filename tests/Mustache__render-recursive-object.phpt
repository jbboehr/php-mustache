--TEST--
Mustache::render() member function - will not crash if data has circular reference
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$data = new stdClass;
$data->var = $data;
$r = $m->render('{{var}}', $data);
var_dump($r);
?>
--EXPECTF--
Warning: Data includes circular reference in %s on line 5
string(0) ""