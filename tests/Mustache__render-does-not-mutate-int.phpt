--TEST--
Mustache::render() member function - Will not mutate int
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$data = new stdClass;
$data->var = 1;
$r = $m->render('{{var}}', $data);
var_dump($r);
var_dump(strval(1)); // verify rendered template matches strval() output
var_dump($data->var);
?>
--EXPECT--
string(1) "1"
string(1) "1"
int(1)