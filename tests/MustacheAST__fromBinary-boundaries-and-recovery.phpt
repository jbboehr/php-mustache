--TEST--
MustacheAST::fromBinary rejects structured invalid boundaries and remains reusable
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$binary = $mustache->parse('Hello {{name}}')->toBinary();
$invalidBinaries = [];

for ($length = 0; $length < strlen($binary); ++$length) {
    $invalidBinaries[] = substr($binary, 0, $length);
}

$invalidBinaries[] = $binary . "\0";
$invalidBinaries[] = $binary . $binary;
$invalidBinaries[] = "X" . substr($binary, 1);

$rejected = 0;
$recovered = 0;

foreach ($invalidBinaries as $invalidBinary) {
    try {
        MustacheAST::fromBinary($invalidBinary);
    } catch (MustacheException $exception) {
        ++$rejected;
    }

    if (MustacheAST::fromBinary($binary)->toBinary() === $binary) {
        ++$recovered;
    }
}

$bounded = false;

try {
    MustacheAST::fromBinary(str_repeat("\0", 16 * 1024 * 1024 + 1));
} catch (MustacheException $exception) {
    $bounded = str_contains($exception->getMessage(), 'size limit exceeded');
}

var_dump($rejected === count($invalidBinaries));
var_dump($recovered === count($invalidBinaries));
var_dump($bounded);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
