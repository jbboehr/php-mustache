--TEST--
Mustache reports the complete built-in INI defaults
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
var_dump(ini_get('mustache.default_escape'));
var_dump(ini_get('mustache.default_start'));
var_dump(ini_get('mustache.default_stop'));
?>
--EXPECT--
string(1) "1"
string(2) "{{"
string(2) "}}"
