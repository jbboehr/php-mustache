--TEST--
MustacheAST::fromBinary safely rejects an abstract late-static subclass
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
abstract class AbstractApplicationAST extends MustacheAST
{
}

$binary = (new Mustache())->parse('Hello {{name}}')->toBinary();

try {
    AbstractApplicationAST::fromBinary($binary);
    echo "accepted\n";
} catch (Throwable $exception) {
    echo "rejected safely\n";
}

echo "survived\n";
?>
--EXPECT--
rejected safely
survived
