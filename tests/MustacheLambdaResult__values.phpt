--TEST--
Explicit lambda results own immutable text, clone by value, and reject unsupported construction and persistence
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip mustache extension required'); ?>
--FILE--
<?php
if (!class_exists('MustacheLiteralResult') || !class_exists('MustacheTemplateResult')) die("missing explicit lambda results\n");
function rejection(string $label, callable $fn): void {
    try { $fn(); echo "$label:accepted\n"; }
    catch (Throwable $e) { echo "$label:", get_class($e), "\n"; }
}
foreach ([MustacheLiteralResult::class, MustacheTemplateResult::class] as $class) {
    echo "$class\n";
    $reflection = new ReflectionClass($class);
    $constructor = $reflection->getConstructor();
    $getter = $reflection->getMethod('getText');
    var_dump($reflection->isFinal(), $reflection->getParentClass() === false,
        $reflection->getInterfaceNames(), $constructor->getNumberOfRequiredParameters(),
        $constructor->getParameters()[0]->getName(), (string) $constructor->getParameters()[0]->getType(),
        (string) $getter->getReturnType(), $getter->getNumberOfParameters());
    $text = "a\0{{name}}";
    $value = new $class(text: $text);
    $text[0] = 'z';
    $read = $value->getText();
    $read[0] = 'y';
    echo bin2hex($value->getText()), "\n";
    $copy = clone $value;
    var_dump($copy == $value, $copy === $value, $value == new $class("a\0{{name}}"),
        $value == new $class('different'), new $class('01') == new $class('1'));
    $weak = WeakReference::create($value);
    unset($value);
    var_dump($weak->get());
    echo bin2hex($copy->getText()), "\n";
    rejection('repeat constructor', fn () => $copy->__construct('changed'));
    echo bin2hex($copy->getText()), "\n";
    rejection('dynamic property', function () use ($copy) { $copy->text = 'changed'; });
    rejection('property reference', function () use ($copy) { $ref =& $copy->text; });
    rejection('missing argument', fn () => new $class());
    rejection('invalid argument', fn () => new $class([]));
    rejection('constructor bypass', fn () => $reflection->newInstanceWithoutConstructor());
    rejection('string conversion', fn () => (string) $copy);
    rejection('serialize', fn () => serialize($copy));
    set_error_handler(function () { return true; });
    try {
        $restored = unserialize(sprintf('O:%d:"%s":0:{}', strlen($class), $class));
        echo $restored === false ? "unserialize:rejected\n" : "unserialize:accepted\n";
    } catch (Throwable $e) { echo "unserialize:rejected\n"; }
    finally { restore_error_handler(); }
    echo 'empty:[', (new $class(''))->getText(), "]\n";
}
var_dump(new MustacheLiteralResult('same') == new MustacheTemplateResult('same'));
?>
--EXPECT--
MustacheLiteralResult
bool(true)
bool(true)
array(0) {
}
int(1)
string(4) "text"
string(6) "string"
string(6) "string"
int(0)
61007b7b6e616d657d7d
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
NULL
61007b7b6e616d657d7d
repeat constructor:Error
61007b7b6e616d657d7d
dynamic property:Error
property reference:Error
missing argument:ArgumentCountError
invalid argument:TypeError
constructor bypass:ReflectionException
string conversion:Error
serialize:Exception
unserialize:rejected
empty:[]
MustacheTemplateResult
bool(true)
bool(true)
array(0) {
}
int(1)
string(4) "text"
string(6) "string"
string(6) "string"
int(0)
61007b7b6e616d657d7d
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
NULL
61007b7b6e616d657d7d
repeat constructor:Error
61007b7b6e616d657d7d
dynamic property:Error
property reference:Error
missing argument:ArgumentCountError
invalid argument:TypeError
constructor bypass:ReflectionException
string conversion:Error
serialize:Exception
unserialize:rejected
empty:[]
bool(false)
