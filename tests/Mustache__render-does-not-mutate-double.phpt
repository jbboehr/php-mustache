--TEST--
Mustache::render() member function - Will not mutate double
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$data = new stdClass;
$data->var = 1.2345;
$r = $m->render('{{var}}', $data);
var_dump($r);
var_dump($data->var);
?>
--EXPECT--
string(18) "1.2344999999999999"
float(1.2345)
