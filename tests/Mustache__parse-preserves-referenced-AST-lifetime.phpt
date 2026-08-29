--TEST--
Mustache::parse() preserves referenced AST identity and lifetime
--SKIPIF--
<?php
if (!extension_loaded('mustache') || !class_exists('WeakReference')) die('skip ');
?>
--FILE--
<?php
class ReferencedApplicationAST extends MustacheAST
{
    public $applicationState = 'preserved';

    public function __destruct()
    {
        echo "destroyed\n";
    }
}

$mustache = new Mustache();
$binary = $mustache->parse('Hello {{name}}')->toBinary();
$input = new ReferencedApplicationAST($binary);
$weak = WeakReference::create($input);
$reference =& $input;

$returned = $mustache->parse($reference);
var_dump($returned === $input);
var_dump(get_class($returned));
var_dump($returned->applicationState);

// Overwrite both referenced input slots. The returned zval must own its object.
$reference = null;
var_dump($input);
var_dump($mustache->render($returned, array('name' => 'Ada')));
var_dump($weak->get() === $returned);

unset($returned);
gc_collect_cycles();
var_dump($weak->get() === null);
?>
--EXPECT--
bool(true)
string(24) "ReferencedApplicationAST"
string(9) "preserved"
NULL
string(9) "Hello Ada"
bool(true)
destroyed
bool(true)
