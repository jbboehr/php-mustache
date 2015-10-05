--TEST--
Mustache::render() member function - Will not mutate null
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$data = new stdClass;
$data->var = null;
$r = $m->render('{{var}}', $data);
var_dump($r);
var_dump(strval(null)); // verify rendered template matches strval() output
var_dump($data->var);
?>
--EXPECT--
string(0) ""
string(0) ""
NULL