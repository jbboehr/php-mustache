--TEST--
Mustache::setEscapeByDefault() preserves weak scalar coercion
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
error_reporting(E_ALL & ~E_DEPRECATED);

$mustache = new Mustache();
$values = [
    'zero float' => 0.0,
    'one float' => 1.0,
    'fractional float' => 1.5,
    'zero string' => '0',
    'one string' => '1',
    'negative string' => '-1',
    'null' => null,
];

foreach ($values as $label => $value) {
    $mustache->setEscapeByDefault($value);
    printf("%s: %d\n", $label, $mustache->getEscapeByDefault());
}
?>
--EXPECT--
zero float: 0
one float: 1
fractional float: 1
zero string: 0
one string: 1
negative string: 1
null: 0
