--TEST--
MustacheData rejects invalid, recursive, and over-deep input transactionally
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
set_error_handler(static function ($severity, $message) {
  echo $message, "\n";
});

$data = new MustacheData(['stable' => true]);

$data->__construct([0 => 'numeric', 'key' => 'associative']);
var_dump($data->toValue());

$recursiveArray = [];
$recursiveArray['self'] =& $recursiveArray;
$data->__construct($recursiveArray);
var_dump($data->toValue());

$recursiveObject = new stdClass();
$recursiveObject->self = $recursiveObject;
$data->__construct($recursiveObject);
var_dump($data->toValue());

$shared = ['leaf'];
$sharedData = new MustacheData([&$shared, &$shared]);
var_dump($sharedData->toValue() === [$shared, $shared]);

$deep = 'leaf';
for ($i = 0; $i < 32; ++$i) {
  $deep = [$deep];
}
$data->__construct($deep);
var_dump($data->toValue());

$data->__construct(INF);
var_dump($data->toValue());

restore_error_handler();
?>
--EXPECT--
MustacheData::__construct(): Mixed numeric and associative arrays are not supported
array(1) {
  ["stable"]=>
  bool(true)
}
MustacheData::__construct(): Data includes circular reference
array(1) {
  ["stable"]=>
  bool(true)
}
MustacheData::__construct(): Data includes circular reference
array(1) {
  ["stable"]=>
  bool(true)
}
bool(true)
MustacheData::__construct(): Data nesting limit exceeded
array(1) {
  ["stable"]=>
  bool(true)
}
MustacheData::__construct(): Non-finite floating-point data is not supported
array(1) {
  ["stable"]=>
  bool(true)
}
