--TEST--
MustacheData charges complete containers before converting their entries
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
set_error_handler(static function ($severity, $message) {
    echo $message, "\n";
});

function tryData(string $label, $value): void {
    echo $label, "\n";
    new MustacheData($value);
}

tryData('exact-limit-mixed', ['named' => null] + array_fill(0, 99999, null));
tryData('over-limit-mixed', ['named' => null] + array_fill(0, 100000, null));

$resource = fopen(__FILE__, 'rb');
$invalidChild = array_fill(0, 100001, null);
$invalidChild[0] = $resource;
tryData('over-limit-invalid-child', $invalidChild);
fclose($resource);
unset($invalidChild);

$resource = fopen(__FILE__, 'rb');
$nested = array_fill(0, 99999, null);
$nested[0] = $resource;
tryData('nested-arrays', [null, $nested]);
fclose($resource);
unset($nested);

$shared = array_fill(0, 50000, null);
tryData('repeated-shared-array', [&$shared, &$shared]);
unset($shared);

$objectWithArray = new stdClass();
$objectWithArray->prefix = null;
$objectWithArray->nested = array_fill(0, 99999, null);
tryData('object-properties-then-array', $objectWithArray);
unset($objectWithArray);

class PropertyAndMethodContext {
    public $property = null;

    public function method() {
        return 'method';
    }
}

$arrayWithObject = array_fill(0, 99999, null);
$arrayWithObject[0] = new PropertyAndMethodContext();
tryData('array-then-object-property-and-method', $arrayWithObject);

restore_error_handler();
?>
--EXPECT--
exact-limit-mixed
MustacheData::__construct(): Mixed numeric and associative arrays are not supported
over-limit-mixed
MustacheData::__construct(): Data container entry limit exceeded
over-limit-invalid-child
MustacheData::__construct(): Data container entry limit exceeded
nested-arrays
MustacheData::__construct(): Data container entry limit exceeded
repeated-shared-array
MustacheData::__construct(): Data container entry limit exceeded
object-properties-then-array
MustacheData::__construct(): Data container entry limit exceeded
array-then-object-property-and-method
MustacheData::__construct(): Data container entry limit exceeded
