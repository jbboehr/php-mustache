--TEST--
Mustache::render() member function - tag
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$r = $m->render('{{var}}', array(
  'var' => 'val',
));
var_dump($r);
?>
--EXPECT--
string(3) "val"