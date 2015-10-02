--TEST--
Mustache::render() member function - Will not mutate true
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$data = new stdClass;
$data->var = true;
$r = $m->render('{{var}}', $data);
var_dump($r);
var_dump(strval(true)); // verify rendered template matches strval() output
var_dump($data->var);
?>
--EXPECT--
string(1) "1"
string(1) "1"
bool(true)