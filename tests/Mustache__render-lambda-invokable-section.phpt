--TEST--
Mustache::render() member function - will render a lambda, provided as an invokable, as a section
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
class Invokable {
  public function __invoke($text) { return strtoupper($text); }
}

$m = new Mustache();
$data = array('upper' => new Invokable());
$r = $m->render('{{#upper}}value{{/upper}}', $data);
var_dump($r);
?>
--EXPECT--
string(5) "VALUE"