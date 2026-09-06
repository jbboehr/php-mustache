--TEST--
Mustache releases sibling captures after lazy initializer exceptions
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
if (PHP_VERSION_ID < 80400) die('skip lazy objects require PHP 8.4');
?>
--FILE--
<?php
class ThrowingCaptureContext
{
    public string $name;
}

class CapturedAfterThrow
{
    public string $name = 'Ada';
}

$mustache = new Mustache();
$reflection = new ReflectionClass(ThrowingCaptureContext::class);
foreach (['ghost', 'proxy'] as $mode) {
    $record = new CapturedAfterThrow();
    $alias = $record;
    $weak = WeakReference::create($record);
    $expected = new RuntimeException('initialization failed');
    $initializer = static function (ThrowingCaptureContext $object) use ($expected) {
        throw $expected;
    };
    $context = $mode === 'ghost'
        ? $reflection->newLazyGhost($initializer)
        : $reflection->newLazyProxy($initializer);
    $input = [$context, $record, &$alias, $record];

    echo "$mode\n";
    try {
        new MustacheData($input);
        echo "exception missing\n";
    } catch (Throwable $error) {
        echo $error === $expected ? 'original exception' : get_class($error);
        echo "\n";
    }
    var_dump($reflection->isUninitializedLazyObject($context));

    // The exception trace and failed lazy object can retain the input independently.
    unset($error, $expected, $initializer, $context, $input, $record, $alias);
    gc_collect_cycles();
    var_dump($weak->get());
    var_dump($mustache->render('{{name}}', ['name' => 'Ada']));
}
?>
--EXPECT--
ghost
original exception
bool(true)
NULL
string(3) "Ada"
proxy
original exception
bool(true)
NULL
string(3) "Ada"
