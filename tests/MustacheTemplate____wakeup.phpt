--TEST--
MustacheTemplate::__wakeup() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = new MustacheTemplate('{{test}}');
$tmpl = unserialize(serialize($tmpl));
var_dump($tmpl);
var_dump($tmpl->__toString());
var_dump($m->render($tmpl, array('test' => 'baz')));
?>
--EXPECT--
object(MustacheTemplate)#3 (1) {
  ["template":protected]=>
  string(8) "{{test}}"
}
string(8) "{{test}}"
string(3) "baz"