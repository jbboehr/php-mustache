--TEST--
Mustache::parse() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$mustache = new Mustache();

$sourceAST = $mustache->parse('Hello {{name}}');
var_dump(get_class($sourceAST));
var_dump($mustache->render($sourceAST, ['name' => 'source']));

$templateAST = $mustache->parse(new MustacheTemplate('Hello {{name}}'));
var_dump($templateAST instanceof MustacheAST);
if ($templateAST instanceof MustacheAST) {
    var_dump($mustache->render($templateAST, ['name' => 'template']));
}

var_dump($mustache->parse($sourceAST) === $sourceAST);
?>
--EXPECT--
string(11) "MustacheAST"
string(12) "Hello source"
bool(true)
string(14) "Hello template"
bool(true)
