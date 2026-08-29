--TEST--
Mustache archived-template benchmark bridge is absent from ordinary builds
--SKIPIF--
<?php
if (!extension_loaded('mustache')) {
    die('skip mustache extension not loaded');
}
if (method_exists(Mustache::class, 'benchmarkSerializeArchive')
    || method_exists(Mustache::class, 'benchmarkRenderArchive')) {
    die('skip benchmark bridge enabled');
}
?>
--FILE--
<?php
var_dump(method_exists(Mustache::class, 'benchmarkSerializeArchive'));
var_dump(method_exists(Mustache::class, 'benchmarkRenderArchive'));
?>
--EXPECT--
bool(false)
bool(false)
