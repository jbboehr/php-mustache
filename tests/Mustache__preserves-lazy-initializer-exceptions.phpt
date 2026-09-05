--TEST--
Mustache propagates lazy initializer exceptions before compiling templates and remains usable
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
if (PHP_VERSION_ID < 80400) die('skip lazy objects require PHP 8.4');
?>
--FILE--
<?php
class LazyContext
{
    public function __construct(public string $name)
    {
    }
}

$mustache = new Mustache();
$reflection = new ReflectionClass(LazyContext::class);
$templates = [
    'source' => 'Hello {{name}}',
    'invalid-source' => '{{#unclosed}}',
    'ast' => $mustache->parse('Hello {{name}}'),
];
foreach (['ghost', 'proxy'] as $mode) {
    foreach ($templates as $label => $template) {
        $attempts = 0;
        $expected = new RuntimeException('initialization failed');
        $initializer = static function (LazyContext $object) use (&$attempts, $expected) {
            $attempts++;
            throw $expected;
        };
        $context = $mode === 'ghost'
            ? $reflection->newLazyGhost($initializer)
            : $reflection->newLazyProxy($initializer);

        echo $mode, '/', $label, "\n";
        try {
            $mustache->render($template, $context);
            echo "exception missing\n";
        } catch (Throwable $error) {
            var_dump($error === $expected);
        }
        var_dump($reflection->isUninitializedLazyObject($context));
        var_dump($mustache->render('Hello {{name}}', new LazyContext('Ada')));
        var_dump($attempts);
    }
}
?>
--EXPECT--
ghost/source
bool(true)
bool(true)
string(9) "Hello Ada"
int(1)
ghost/invalid-source
bool(true)
bool(true)
string(9) "Hello Ada"
int(1)
ghost/ast
bool(true)
bool(true)
string(9) "Hello Ada"
int(1)
proxy/source
bool(true)
bool(true)
string(9) "Hello Ada"
int(1)
proxy/invalid-source
bool(true)
bool(true)
string(9) "Hello Ada"
int(1)
proxy/ast
bool(true)
bool(true)
string(9) "Hello Ada"
int(1)
