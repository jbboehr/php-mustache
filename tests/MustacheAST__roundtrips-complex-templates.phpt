--TEST--
MustacheAST round-trips complex templates and renders with AST partials
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$source = "A\0{{name}}{{^missing}}B{{/missing}}{{#section}}C{{/section}}{{! ignored}}{{>partial}}";
$ast = $mustache->parse($source);
$tree = $ast->toArray();
$binary = (string) $ast;

$copy = new MustacheAST($binary);
var_dump($copy->toArray() === $tree);
var_dump((string) $copy === $binary);

$serializedCopy = unserialize(serialize($ast));
var_dump($serializedCopy->toArray() === $tree);

$partial = $mustache->parse('P{{{name}}}');
$output = $mustache->render($copy, [
  'name' => '<N>',
  'missing' => false,
  'section' => true,
], [
  'partial' => $partial,
]);
var_dump(strlen($output));
var_dump(bin2hex($output));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
int(17)
string(34) "4100266c743b4e2667743b4243503c4e3e"
