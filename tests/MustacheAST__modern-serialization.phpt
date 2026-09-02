--TEST--
MustacheAST uses modern PHP serialization hooks and reads legacy payloads
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class LegacyPublicAST extends MustacheAST
{
    public $binaryString;
}

$mustache = new Mustache();
$ast = $mustache->parse('Hello {{name}}');
$binary = $ast->toBinary();

var_dump(method_exists($ast, '__serialize'));
var_dump(method_exists($ast, '__unserialize'));
var_dump(method_exists($ast, '__sleep'));
var_dump(method_exists($ast, '__wakeup'));
var_dump($ast->__serialize() === ['binary' => $binary]);

$serialized = serialize($ast);
var_dump(str_contains($serialized, 's:6:"binary";'));
var_dump(!str_contains($serialized, 'binaryString'));

$copy = unserialize($serialized);
var_dump($mustache->render($copy, ['name' => 'new']));

$legacyKey = "\0*\0binaryString";
$legacyPayload = sprintf(
    'O:11:"MustacheAST":1:{s:%d:"%s";s:%d:"%s";}',
    strlen($legacyKey),
    $legacyKey,
    strlen($binary),
    $binary,
);
$legacyCopy = unserialize($legacyPayload);
var_dump($mustache->render($legacyCopy, ['name' => 'legacy']));

$publicLegacyKey = 'binaryString';
$publicLegacyPayload = sprintf(
    'O:%d:"%s":1:{s:%d:"%s";s:%d:"%s";}',
    strlen(LegacyPublicAST::class),
    LegacyPublicAST::class,
    strlen($publicLegacyKey),
    $publicLegacyKey,
    strlen($binary),
    $binary,
);
$publicLegacyCopy = unserialize($publicLegacyPayload);
var_dump(get_class($publicLegacyCopy));
var_dump($mustache->render($publicLegacyCopy, ['name' => 'public']));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
string(9) "Hello new"
string(12) "Hello legacy"
string(15) "LegacyPublicAST"
string(12) "Hello public"
