--TEST--
MustacheAST serialization preserves subclass, alias, and exception recovery semantics
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class ApplicationAST extends MustacheAST
{
    public string $marker = 'default';
    public int $unserializeCalls = 0;

    public function __unserialize(array $data): void
    {
        ++$this->unserializeCalls;
        parent::__unserialize($data);
    }
}

class CapturingAST extends MustacheAST
{
    public static $captured;

    public function __unserialize(array $data): void
    {
        self::$captured = $this;
        parent::__unserialize($data);
    }
}

function serializedASTWire(string $class, string $binary): string
{
    return sprintf(
        'O:%d:"%s":1:{s:6:"binary";s:%d:"%s";}',
        strlen($class),
        $class,
        strlen($binary),
        $binary,
    );
}

$mustache = new Mustache();
$binary = $mustache->parse('Hello {{name}}')->toBinary();

$original = ApplicationAST::fromBinary($binary);
$original->marker = 'runtime-only';
$graph = unserialize(serialize([$original, $original]));
var_dump($graph[0] === $graph[1]);
var_dump(get_class($graph[0]));
var_dump($graph[0]->marker);
var_dump($graph[0]->unserializeCalls);
var_dump($mustache->render($graph[0], ['name' => 'alias']));

try {
    unserialize(serializedASTWire(CapturingAST::class, $binary . "\0"));
    echo "invalid binary:accepted\n";
} catch (MustacheException $exception) {
    echo "invalid binary:rejected\n";
}
var_dump(CapturingAST::$captured instanceof CapturingAST);

try {
    serialize(CapturingAST::$captured);
    echo "uninitialized:serialized\n";
} catch (ValueError $exception) {
    echo "uninitialized:rejected\n";
}

CapturingAST::$captured->__unserialize(['binary' => $binary]);
var_dump($mustache->render(CapturingAST::$captured, ['name' => 'recovered']));
?>
--EXPECT--
bool(true)
string(14) "ApplicationAST"
string(7) "default"
int(1)
string(11) "Hello alias"
invalid binary:rejected
bool(true)
uninitialized:rejected
string(15) "Hello recovered"
