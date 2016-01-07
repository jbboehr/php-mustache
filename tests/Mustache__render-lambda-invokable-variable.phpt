--TEST--
Mustache::render() member function - will render a lambda, provided as an invokable, as a variable
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
class Invokable {
  public function __invoke() { return 'value'; }
}

$m = new Mustache();
$data = array('var' => new Invokable());
$r = $m->render('{{var}}', $data);
var_dump($r);
?>
--EXPECT--
string(5) "value"