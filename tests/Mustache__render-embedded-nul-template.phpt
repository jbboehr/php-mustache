--TEST--
Mustache::render() preserves embedded NUL bytes in template source
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$result = $mustache->render("A\0B{{value}}", array('value' => 'C'));
var_dump(bin2hex($result));
?>
--EXPECT--
string(8) "41004243"
