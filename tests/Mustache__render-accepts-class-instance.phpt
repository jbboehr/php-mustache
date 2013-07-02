--TEST--
Mustache::render() member function - Accepts class instance
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
class SomeClass
{
  public $bar = 'baz';
}
$m = new Mustache();
$r = $m->render('{{bar}}', new SomeClass);
var_dump($r);
$r = $m->render('{{#foo}}{{bar}}{{/foo}}', array('foo' => array(new SomeClass)));
var_dump($r);
$r = $m->render('{{#foo}}{{bar}}{{/foo}}', array('foo' => array('bar' => new SomeClass)));
var_dump($r);
?>
--EXPECT--
string(3) "baz"
string(3) "baz"
string(0) ""