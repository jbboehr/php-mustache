--TEST--
Mustache::render() member function - Accepts MustacheData
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$data = new MustacheData(array(
  'var' => 'val',
));
$r = $m->render('{{var}}', $data);
var_dump($r);
?>
--EXPECT--
string(3) "val"