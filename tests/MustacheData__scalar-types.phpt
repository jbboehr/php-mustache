--TEST--
MustacheData preserves scalar types, empty arrays, and embedded NUL bytes
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
$key = "a\0b";
$value = "x\0y";
$data = new MustacheData([
  null,
  false,
  true,
  42,
  1.2345,
  $value,
  [],
  [$key => $value],
]);
$result = $data->toValue();

var_dump(array_slice($result, 0, 5));
var_dump(bin2hex($result[5]));
var_dump($result[6]);
var_dump(bin2hex(array_key_first($result[7])));
var_dump(bin2hex($result[7][$key]));
?>
--EXPECT--
array(5) {
  [0]=>
  NULL
  [1]=>
  bool(false)
  [2]=>
  bool(true)
  [3]=>
  int(42)
  [4]=>
  float(1.2345)
}
string(6) "780079"
array(0) {
}
string(6) "610062"
string(6) "780079"
