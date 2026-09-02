--TEST--
Mustache archived-template benchmark bridge accepts MustacheAST partials
--SKIPIF--
<?php
if (!extension_loaded('mustache')) {
    die('skip mustache extension not loaded');
}
if (!method_exists(Mustache::class, 'benchmarkSerializeArchive')
    || !method_exists(Mustache::class, 'benchmarkRenderArchive')) {
    die('skip benchmark bridge disabled');
}
?>
--FILE--
<?php
$mustache = new Mustache();
$template = '{{>card}}';
$partials = ['card' => $mustache->parse('AST {{name}}')];
$data = ['name' => 'Ada'];

$archive = $mustache->benchmarkSerializeArchive($template, $partials);

var_dump($mustache->benchmarkRenderArchive($archive, $data));
?>
--EXPECT--
string(7) "AST Ada"
