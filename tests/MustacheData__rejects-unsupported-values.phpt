--TEST--
MustacheData rejects unsupported values and reports lossy lambda conversion
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
set_error_handler(static function ($severity, $message) {
  echo $message, "\n";
});

$stableData = new MustacheData(['stable' => true]);
$nestedData = new MustacheData(['nested' => $stableData]);
var_dump($nestedData->toValue());

$resource = fopen('php://memory', 'r');
$resourceData = new MustacheData($resource);
var_dump($resourceData->toValue());
try {
  (new Mustache())->render('unused', $resource);
} catch (Throwable $error) {
  echo get_class($error), "\n";
}
fclose($resource);

try {
  (new ReflectionClass(MustacheData::class))->newInstanceWithoutConstructor();
  echo "constructor bypass accepted\n";
} catch (ReflectionException $error) {
  echo get_class($error), "\n";
}
try {
  (new Mustache())->render('unused', $resourceData);
} catch (Throwable $error) {
  echo get_class($error), "\n";
}

$lambdaData = new MustacheData([
  'closure' => static function () {
    return 'closure';
  },
  'invokable' => new class {
    public function __invoke() {
      return 'invokable';
    }
  },
]);
$lambdaValues = $lambdaData->toValue();
ksort($lambdaValues);
var_dump($lambdaValues);

restore_error_handler();
?>
--EXPECT--
MustacheData::__construct(): Nested MustacheData values are not supported
MustacheData::toValue(): MustacheData was not initialized properly
bool(false)
MustacheData::__construct(): Invalid data type
MustacheData::toValue(): MustacheData was not initialized properly
bool(false)
TypeError
ReflectionException
ValueError
Lambda data cannot be converted to a PHP value
Lambda data cannot be converted to a PHP value
array(2) {
  ["closure"]=>
  NULL
  ["invokable"]=>
  NULL
}
