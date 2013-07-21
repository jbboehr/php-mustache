--TEST--
Mustache::execute() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = $m->compile('{{test}}');
$output = $m->execute($tmpl, array('test' => 'baz'));
var_dump($output);
?>
--EXPECT--
string(3) "baz"