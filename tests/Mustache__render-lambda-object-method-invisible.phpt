--TEST--
Mustache::render() member function - will exclude private/protected properties and methods from context
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
class Data {
  private $a = 'a';
  protected $b = 'b';

  private function c() { return 'c'; }
  protected function d() { return 'd'; }
}

$m = new Mustache();
$r = $m->render('{{a}}{{b}}{{c}}{{d}}', new Data());
var_dump($r);
?>
--EXPECT--
string(0) ""