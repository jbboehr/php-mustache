--TEST--
Mustache::render() member function - will render a lambda, provided as a closure, as a section
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$data = array('upper' => function ($text) { return strtoupper($text); });
$r = $m->render('{{#upper}}value{{/upper}}', $data);
var_dump($r);
?>
--EXPECT--
string(5) "VALUE"