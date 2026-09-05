--TEST--
Mustache initializes declared subclass properties and keeps instance defaults independent
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class GreetingEngine extends Mustache
{
    public $label = 'engine';
    protected string $greeting = 'Hello';
    private string $punctuation = '!';

    public function welcome(string $name): string
    {
        return $this->render($this->greeting . ' {{name}}' . $this->punctuation, ['name' => $name]);
    }
}

class ApplicationEngine extends GreetingEngine
{
    public string $name = 'Ada';
    public array $context = ['name' => 'Grace'];
    public ?string $optional = null;
    public string $uninitialized;
}

try {
    $first = new ApplicationEngine();
    var_dump($first->label, $first->name, $first->optional);
    var_dump($first->welcome($first->context['name']));
    var_dump((new ReflectionProperty(ApplicationEngine::class, 'uninitialized'))->isInitialized($first));

    $first->label = 'changed';
    $first->name = 'Linus';
    $first->context['name'] = 'Linus';

    $second = new ApplicationEngine();
    var_dump($first->welcome($first->context['name']));
    unset($first);
    var_dump($second->label, $second->name, $second->context['name']);
    var_dump($second->welcome($second->name));
} catch (Error $error) {
    echo $error->getMessage(), "\n";
}
?>
--EXPECT--
string(6) "engine"
string(3) "Ada"
NULL
string(12) "Hello Grace!"
bool(false)
string(12) "Hello Linus!"
string(6) "engine"
string(3) "Ada"
string(5) "Grace"
string(10) "Hello Ada!"
