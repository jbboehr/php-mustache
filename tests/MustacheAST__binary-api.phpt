--TEST--
MustacheAST exposes explicit binary import and export APIs
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$ast = $mustache->parse('Hello {{name}}');
$binary = $ast->toBinary();

var_dump($binary === (string) $ast);

$copy = MustacheAST::fromBinary($binary);
var_dump(get_class($copy));
var_dump($copy->toArray() === $ast->toArray());
var_dump($mustache->render($copy, ['name' => 'world']));

class ApplicationAST extends MustacheAST
{
    public function __construct()
    {
        throw new Exception('The factory must not invoke subclass constructors');
    }
}

$derived = ApplicationAST::fromBinary($binary);
var_dump(get_class($derived));
var_dump($mustache->render($derived, ['name' => 'subclass']));

foreach ([substr($binary, 0, -1), $binary . "\0"] as $invalid) {
    try {
        MustacheAST::fromBinary($invalid);
        echo "accepted\n";
    } catch (MustacheException $exception) {
        echo "rejected\n";
    }
}
?>
--EXPECT--
bool(true)
string(11) "MustacheAST"
bool(true)
string(11) "Hello world"
string(14) "ApplicationAST"
string(14) "Hello subclass"
rejected
rejected
