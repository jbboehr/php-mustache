--TEST--
Mustache::render() member function - will render a lambda, provided as a closure, as a variable
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$data = array('var' => function () { return 'value'; });
$r = $m->render('{{var}}', $data);
var_dump($r);
?>
--EXPECT--
string(5) "value"