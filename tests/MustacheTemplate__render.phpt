--TEST--
MustacheTemplate::render() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$tmpl = new MustacheTemplate('{{test}}');
var_dump($tmpl->render(array(
  'test' => 'okay'
)));
?>
--EXPECT--
string(4) "okay"