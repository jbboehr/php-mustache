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

$mixedData = new MustacheData([0 => 'numeric', 'key' => 'associative']);
var_dump($mixedData->toValue());

$recursiveArray = [];
$recursiveArray['self'] =& $recursiveArray;
$recursiveArrayData = new MustacheData($recursiveArray);
var_dump($recursiveArrayData->toValue());

$recursiveObject = new stdClass();
$recursiveObject->self = $recursiveObject;
$recursiveObjectData = new MustacheData($recursiveObject);
var_dump($recursiveObjectData->toValue());

$shared = ['leaf'];
$sharedData = new MustacheData([&$shared, &$shared]);
var_dump($sharedData->toValue() === [$shared, $shared]);

$deep = 'leaf';
for ($i = 0; $i < 32; ++$i) {
  $deep = [$deep];
}
$deepData = new MustacheData($deep);
var_dump($deepData->toValue());

$infiniteData = new MustacheData(INF);
var_dump($infiniteData->toValue());

restore_error_handler();
unset($recursiveArray, $recursiveObject);
gc_collect_cycles();
?>
--EXPECT--
MustacheData::__construct(): Mixed numeric and associative arrays are not supported
MustacheData::toValue(): MustacheData was not initialized properly
bool(false)
MustacheData::__construct(): Data includes circular reference
MustacheData::toValue(): MustacheData was not initialized properly
bool(false)
MustacheData::__construct(): Data includes circular reference
MustacheData::toValue(): MustacheData was not initialized properly
bool(false)
bool(true)
MustacheData::__construct(): Data nesting limit exceeded
MustacheData::toValue(): MustacheData was not initialized properly
bool(false)
MustacheData::__construct(): Non-finite floating-point data is not supported
MustacheData::toValue(): MustacheData was not initialized properly
bool(false)
