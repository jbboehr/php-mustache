--TEST--
MustacheAST::__unserialize() validates payloads and recovers transactionally
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
function newUninitializedAST(): MustacheAST
{
    return (new ReflectionClass(MustacheAST::class))->newInstanceWithoutConstructor();
}

$mustache = new Mustache();
$binary = $mustache->parse('{{name}}')->toBinary();
$legacyKey = "\0*\0binaryString";

$invalidPayloads = [
    'missing' => [],
    'wrong type' => ['binary' => 123],
    'extra key' => ['binary' => $binary, 'extra' => true],
    'ambiguous' => ['binary' => $binary, $legacyKey => $binary],
];

foreach ($invalidPayloads as $label => $payload) {
    try {
        newUninitializedAST()->__unserialize($payload);
        echo "$label:accepted\n";
    } catch (ValueError $exception) {
        echo "$label:rejected\n";
    }
}

$referencedBinary = $binary;
$referencedPayload = ['binary' => &$referencedBinary];
$referenced = newUninitializedAST();
$referenced->__unserialize($referencedPayload);
$referencedBinary = '';
unset($referencedPayload);
var_dump($mustache->render($referenced, ['name' => 'referenced']));

$recovering = newUninitializedAST();
try {
    $recovering->__unserialize(['binary' => $binary . "\0"]);
    echo "invalid binary:accepted\n";
} catch (MustacheException $exception) {
    echo "invalid binary:rejected\n";
}
$recovering->__unserialize(['binary' => $binary]);
var_dump($mustache->render($recovering, ['name' => 'recovered']));

$initialized = $mustache->parse('{{name}}');
try {
    $initialized->__unserialize(['binary' => $binary]);
    echo "reinitialization:accepted\n";
} catch (ValueError $exception) {
    echo "reinitialization:rejected\n";
}
var_dump($mustache->render($initialized, ['name' => 'unchanged']));
?>
--EXPECT--
missing:rejected
wrong type:rejected
extra key:rejected
ambiguous:rejected
string(10) "referenced"
invalid binary:rejected
string(9) "recovered"
reinitialization:rejected
string(9) "unchanged"
