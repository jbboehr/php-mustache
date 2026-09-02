--TEST--
MustacheAST serialization bounds native output and leaves the AST reusable
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--INI--
memory_limit=256M
--FILE--
<?php
$limit = 16 * 1024 * 1024;
$mustache = new Mustache();

$below = $mustache->parse(str_repeat('x', $limit - 1024));
$payload = $below->__serialize();
var_dump(array_keys($payload) === ['binary']);
var_dump(strlen($payload['binary']) <= $limit);

$over = $mustache->parse(str_repeat('x', $limit));
try {
    serialize($over);
    echo "oversized:accepted\n";
} catch (MustacheException $exception) {
    echo "oversized:rejected\n";
}
var_dump(strlen($mustache->render($over, [])) === $limit);

$small = unserialize(serialize($mustache->parse('{{value}}')));
var_dump($mustache->render($small, ['value' => 'recovered']));
?>
--EXPECT--
bool(true)
bool(true)
oversized:rejected
bool(true)
string(9) "recovered"
