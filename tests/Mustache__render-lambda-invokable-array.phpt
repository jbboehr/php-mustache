--TEST--
Mustache::render() member function - will render a list of lambdas, provided as invokables, as variables
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
class Invokable {
  private $value;
  public function __construct($value) { $this->value = $value; }
  public function __invoke() { return $this->value; }
}

$m = new Mustache();
$data = array('list' => array(new Invokable(1), new Invokable(2), new Invokable(3)));
$r = $m->render('{{#list}}{{.}}{{/list}}', $data);
var_dump($r);
?>
--EXPECT--
string(3) "123"