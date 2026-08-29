--TEST--
Mustache::setEscapeByDefault() accepts its reflected bool|int type under strict_types
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
declare(strict_types=1);

$mustache = new Mustache();

foreach ([true, false, 1, 0] as $value) {
    var_dump($mustache->setEscapeByDefault(escapeByDefault: $value));
    var_dump($mustache->getEscapeByDefault());
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
