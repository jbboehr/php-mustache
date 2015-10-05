--TEST--
Mustache::render() member function - Will not mutate false
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$data = new stdClass;
$data->var = false;
$r = $m->render('{{var}}', $data);
var_dump($r);
var_dump(strval(false)); // verify rendered template matches strval() output
var_dump($data->var);
?>
--EXPECT--
string(0) ""
string(0) ""
bool(false)