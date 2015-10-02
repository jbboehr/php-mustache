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
var_dump(strval(1.2345)); // verify rendered template matches strval() output
var_dump($data->var);
?>
--EXPECT--
string(6) "1.2345"
string(6) "1.2345"
float(1.2345)