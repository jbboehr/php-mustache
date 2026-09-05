--TEST--
MustacheAST accessors reject uninitialized state without warnings and recover after initialization
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
function checkUninitializedAccessors($label, MustacheAST $ast)
{
    $accessors = [
        'toArray' => fn() => $ast->toArray(),
        'toBinary' => fn() => $ast->toBinary(),
        '__toString' => fn() => $ast->__toString(),
        'cast' => fn() => (string) $ast,
        '__serialize' => fn() => $ast->__serialize(),
    ];

    foreach ($accessors as $method => $call) {
        $warnings = 0;
        set_error_handler(function ($severity, $message) use (&$warnings) {
            ++$warnings;
            return true;
        });
        try {
            $value = $call();
            $result = 'returned ' . gettype($value);
        } catch (Throwable $error) {
            $result = get_class($error);
        } finally {
            restore_error_handler();
        }
        echo $label, ' ', $method, ': ', $result, ' warnings=', $warnings, "\n";
    }
}

$ast = (new ReflectionClass(MustacheAST::class))->newInstanceWithoutConstructor();
checkUninitializedAccessors('fresh', $ast);

try {
    $ast->__unserialize(['binary' => 123]);
    echo "invalid payload: accepted\n";
} catch (ValueError $error) {
    echo "invalid payload: ValueError\n";
}
checkUninitializedAccessors('after failed initialization', $ast);

$mustache = new Mustache();
$source = $mustache->parse('Hello {{name}}');
$binary = $source->toBinary();
$ast->__unserialize(['binary' => $binary]);

var_dump($ast->toArray() === $source->toArray());
var_dump($ast->toBinary() === $binary);
var_dump($ast->__toString() === $binary);
var_dump((string) $ast === $binary);
var_dump($ast->__serialize() === ['binary' => $binary]);
echo $mustache->render($ast, ['name' => 'Ada']), "\n";
?>
--EXPECT--
fresh toArray: ValueError warnings=0
fresh toBinary: ValueError warnings=0
fresh __toString: ValueError warnings=0
fresh cast: ValueError warnings=0
fresh __serialize: ValueError warnings=0
invalid payload: ValueError
after failed initialization toArray: ValueError warnings=0
after failed initialization toBinary: ValueError warnings=0
after failed initialization __toString: ValueError warnings=0
after failed initialization cast: ValueError warnings=0
after failed initialization __serialize: ValueError warnings=0
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Hello Ada
