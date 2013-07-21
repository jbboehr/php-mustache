--TEST--
Mustache::execute() member function - Accepts MustacheData
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = $m->compile('{{test}}');
$data = new MustacheData(array('test' => 'baz'));
$output = $m->execute($tmpl, $data);
var_dump($output);
?>
--EXPECT--
string(3) "baz"