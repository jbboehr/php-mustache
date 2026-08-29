--TEST--
MustacheAST explicit null construction matches omission under strict_types
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
declare(strict_types=1);

foreach ([
    'omitted' => static fn () => new MustacheAST(),
    'null' => static fn () => new MustacheAST(null),
] as $label => $construct) {
    try {
        $construct();
        printf("%s: returned\n", $label);
    } catch (Throwable $error) {
        printf("%s: %s\n", $label, get_class($error));
    }
}
?>
--EXPECT--
omitted: MustacheException
null: MustacheException
