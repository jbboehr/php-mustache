--TEST--
Mustache::render() member function - will render lambdas, provided as class methods, as variables
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$tmpl = <<<EOF
a: {{a}}
b: {{b}}
c: {{c}}
parent_public_var: {{parent_public_var}}
upper: {{#upper}}upper{{/upper}}
EOF;

class Parent_Data {
  public $parent_public_var = 'x';

  public function a() { return 'parent_a'; }
  public function b() { return 'parent_b'; }

  public function upper($text) { return strtoupper($text); }
}

class Data extends Parent_Data {
  private $name = 'child';

  public function a() { return 'child_a'; }
  public function c() { return $this->name . '_c'; }
  private function d() { return 'd'; }
}

$m = new Mustache();
$r = $m->render($tmpl, new Data());
var_dump($r);
?>
--EXPECT--
string(67) "a: child_a
b: parent_b
c: child_c
parent_public_var: x
upper: UPPER"