--TEST--
Mustache::execute() member function - With Partials
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = $m->compile('{{test}} {{>foo}}', array(
  'foo' => '{{#foo}}{{bar}}{{/foo}}',
));
$output = $m->execute($tmpl, array('test' => 'baz', 'foo' => array(
    array('bar' => 'A'),
    array('bar' => 'B'),
)));
var_dump($output);
?>
--EXPECT--
string(6) "baz AB"