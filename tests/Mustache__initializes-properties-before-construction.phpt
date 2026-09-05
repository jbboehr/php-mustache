--TEST--
Mustache initializes subclass properties before or without a PHP constructor
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class ConfiguredEngine extends Mustache
{
    public string $start = '<%';
    public string $stop = '%>';
    public int $constructorCalls = 0;

    public function __construct()
    {
        $this->constructorCalls++;
        $this->setStartSequence($this->start);
        $this->setStopSequence($this->stop);
    }
}

try {
    $configured = new ConfiguredEngine();
    var_dump($configured->constructorCalls);
    var_dump($configured->render('<%name%>', ['name' => 'Ada']));
} catch (Error $error) {
    echo $error->getMessage(), "\n";
}

try {
    $withoutConstructor = (new ReflectionClass(ConfiguredEngine::class))->newInstanceWithoutConstructor();
    var_dump($withoutConstructor->start, $withoutConstructor->stop, $withoutConstructor->constructorCalls);
    var_dump($withoutConstructor->render('{{name}}', ['name' => 'Grace']));
} catch (Error $error) {
    echo $error->getMessage(), "\n";
}
?>
--EXPECT--
int(1)
string(3) "Ada"
string(2) "<%"
string(2) "%>"
int(0)
string(5) "Grace"
