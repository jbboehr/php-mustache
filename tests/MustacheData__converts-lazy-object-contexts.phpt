--TEST--
Mustache converts lazy ghosts and proxies as property contexts
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
if (PHP_VERSION_ID < 80400) die('skip lazy objects require PHP 8.4');
?>
--FILE--
<?php
class LazyPerson
{
    public function __construct(public string $name)
    {
    }
}

function lazyPerson(string $mode, int &$initializations): LazyPerson
{
    $reflection = new ReflectionClass(LazyPerson::class);
    $initializer = static function (LazyPerson $object) use ($mode, &$initializations) {
        $initializations++;
        if ($mode === 'proxy') {
            return new LazyPerson('Ada');
        }
        $object->name = 'Ada';
    };
    return $mode === 'ghost'
        ? $reflection->newLazyGhost($initializer)
        : $reflection->newLazyProxy($initializer);
}

$mustache = new Mustache();
$ast = $mustache->parse('{{name}}');
$nestedAST = $mustache->parse('{{person.name}}');
foreach (['ghost', 'proxy'] as $mode) {
    echo $mode, "\n";
    $initializations = 0;
    $person = lazyPerson($mode, $initializations);
    var_dump($initializations);
    var_dump($mustache->render('{{name}}', $person));
    var_dump($mustache->render($ast, $person));
    var_dump($initializations);

    $data = new MustacheData(['person' => lazyPerson($mode, $initializations)]);
    var_dump($data->toValue() === ['person' => ['name' => 'Ada']]);
    var_dump($mustache->render('{{person.name}}', $data));
    var_dump($initializations);

    $context = (object) ['person' => lazyPerson($mode, $initializations)];
    var_dump($mustache->render($nestedAST, $context));
    var_dump($initializations);
}
?>
--EXPECT--
ghost
int(0)
string(3) "Ada"
string(3) "Ada"
int(1)
bool(true)
string(3) "Ada"
int(2)
string(3) "Ada"
int(3)
proxy
int(0)
string(3) "Ada"
string(3) "Ada"
int(1)
bool(true)
string(3) "Ada"
int(2)
string(3) "Ada"
int(3)
