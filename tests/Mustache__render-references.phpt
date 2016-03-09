--TEST--
Mustache::render() member function - references
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$value = 'val';

$m = new Mustache();
$r = $m->render('{{var}}', array(
  'var' => &$value,
));
var_dump($r);

?>
--EXPECT--
string(3) "val"
