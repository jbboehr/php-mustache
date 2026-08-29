--TEST--
MustacheTemplate accepts its explicit nullable default under strict_types
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
declare(strict_types=1);

var_dump(get_class(new MustacheTemplate()));
var_dump(get_class(new MustacheTemplate(null)));
?>
--EXPECT--
string(16) "MustacheTemplate"
string(16) "MustacheTemplate"
