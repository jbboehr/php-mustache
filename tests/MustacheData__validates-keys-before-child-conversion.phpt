--TEST--
Mustache rejects mixed array keys before running a child's lazy initializer
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
if (PHP_VERSION_ID < 80400) die('skip lazy objects require PHP 8.4');
?>
--FILE--
<?php
class KeyValidationContext
{
    public string $name;
}

$mustache = new Mustache();
$reflection = new ReflectionClass(KeyValidationContext::class);
$templates = ['source' => 'unused', 'invalid-source' => '{{#unclosed}}', 'ast' => $mustache->parse('unused')];
foreach (['ghost', 'proxy'] as $mode) {
    foreach ($templates as $path => $template) {
        $attempts = 0;
        $initializer = static function (KeyValidationContext $object) use (&$attempts) {
            $attempts++;
            throw new RuntimeException('initializer reached');
        };
        $context = $mode === 'ghost'
            ? $reflection->newLazyGhost($initializer)
            : $reflection->newLazyProxy($initializer);
        echo "$mode/$path\n";
        try {
            $mustache->render($template, ['named' => $context, 0 => null]);
            echo "exception missing\n";
        } catch (Throwable $error) {
            echo get_class($error), "\n";
        }
        var_dump($attempts, $reflection->isUninitializedLazyObject($context));
        var_dump($mustache->render('{{name}}', ['name' => 'Ada']));
    }
}
?>
--EXPECT--
ghost/source
ValueError
int(0)
bool(true)
string(3) "Ada"
ghost/invalid-source
ValueError
int(0)
bool(true)
string(3) "Ada"
ghost/ast
ValueError
int(0)
bool(true)
string(3) "Ada"
proxy/source
ValueError
int(0)
bool(true)
string(3) "Ada"
proxy/invalid-source
ValueError
int(0)
bool(true)
string(3) "Ada"
proxy/ast
ValueError
int(0)
bool(true)
string(3) "Ada"
