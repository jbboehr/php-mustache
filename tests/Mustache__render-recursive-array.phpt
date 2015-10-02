--TEST--
Mustache::render() member function - will not crash if data contains array with circular reference
--SKIPIF--
<?php

// references are a distinct type of zval in PHP7 and they are not supported by this extension
if(!extension_loaded('mustache') || (defined('PHP_VERSION_ID') && PHP_VERSION_ID >= 70000)) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$data = [];
$data['var'] = &$data;
$r = $m->render('{{var}}', $data);
var_dump($r);
?>
--EXPECTF--
Warning: Data includes circular reference in %s on line 5
string(0) ""