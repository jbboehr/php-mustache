--TEST--
MustacheAST rejects truncated and trailing serialization data
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$binary = (string) $mustache->parse('Hello {{name}}');

foreach (array(substr($binary, 0, -1), $binary . "\0") as $invalid) {
    try {
        new MustacheAST($invalid);
        echo "accepted\n";
    } catch (Throwable $e) {
        echo "rejected\n";
    }
}

$roundTrip = new MustacheAST($binary);
var_dump($mustache->render($roundTrip, array('name' => 'world')));
?>
--EXPECT--
rejected
rejected
string(11) "Hello world"
