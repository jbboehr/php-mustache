--TEST--
MustacheData gives public properties precedence over methods with the same name
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
class PropertyMethodCollision {
  public $value = 'property';

  public function value() {
    return 'method';
  }
}

$data = new MustacheData(new PropertyMethodCollision());
var_dump($data->toValue());
?>
--EXPECT--
array(1) {
  ["value"]=>
  string(8) "property"
}
