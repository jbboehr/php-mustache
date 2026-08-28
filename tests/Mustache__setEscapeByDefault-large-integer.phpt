--TEST--
Mustache::setEscapeByDefault() accepts a 64-bit integer
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
if (PHP_INT_SIZE < 8) die('skip requires 64-bit PHP');
?>
--FILE--
<?php
$m = new Mustache();
$m->setEscapeByDefault(4294967296);
var_dump($m->getEscapeByDefault());
?>
--EXPECT--
bool(true)
