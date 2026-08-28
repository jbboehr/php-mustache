--TEST--
Mustache rejects empty INI delimiters without corrupting its defaults
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--INI--
mustache.default_start=
mustache.default_stop=
--FILE--
<?php
var_dump(@ini_set('mustache.default_start', ''));
var_dump(@ini_set('mustache.default_stop', ''));
var_dump(ini_get('mustache.default_start'));
var_dump(ini_get('mustache.default_stop'));

$mustache = new Mustache();
var_dump($mustache->getStartSequence());
var_dump($mustache->getStopSequence());
?>
--EXPECT--
bool(false)
bool(false)
string(2) "{{"
string(2) "}}"
string(2) "{{"
string(2) "}}"
