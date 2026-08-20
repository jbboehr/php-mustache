--TEST--
Mustache::render() member function - will not crash if data has circular reference
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$data = new stdClass;
$data->var = $data;
set_error_handler(static function ($severity, $message) {
  echo $message, "\n";
});
$r = $m->render('{{var}}', $data);
restore_error_handler();
var_dump($r);
?>
--EXPECT--
Mustache::render(): Data includes circular reference
NULL
