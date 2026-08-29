--TEST--
MustacheAST::fromBinary initializes late-static subclass property defaults
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class ApplicationStateAST extends MustacheAST
{
    public string $marker = 'ready';

    public function __construct()
    {
        throw new Exception('The factory must not invoke subclass constructors');
    }
}

$mustache = new Mustache();
$binary = $mustache->parse('Hello {{name}}')->toBinary();
$ast = ApplicationStateAST::fromBinary($binary);

try {
    var_dump($ast->marker);
} catch (Error $exception) {
    echo "property uninitialized\n";
}

var_dump($mustache->render($ast, ['name' => 'world']));
?>
--EXPECT--
string(5) "ready"
string(11) "Hello world"
