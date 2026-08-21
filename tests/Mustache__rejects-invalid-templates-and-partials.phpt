--TEST--
Mustache rejects invalid templates and partials on source and AST render paths
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
set_error_handler(static function ($severity, $message) {
  echo $message, "\n";
});

$mustache = new Mustache();

var_dump($mustache->render(123, []));
var_dump($mustache->render(new stdClass(), []));
var_dump($mustache->render(new MustacheTemplate(), []));
var_dump($mustache->parse(123));

$emptyAST = (new ReflectionClass(MustacheAST::class))->newInstanceWithoutConstructor();
var_dump($mustache->render($emptyAST, []));
var_dump($mustache->parse($emptyAST));

$source = 'A{{>x}}B';
$ast = $mustache->parse($source);
foreach ([$source, $ast] as $template) {
  var_dump($mustache->render($template, [], ['numeric partial']));
  var_dump($mustache->render($template, [], ['x' => 123]));
  var_dump($mustache->render($template, [], ['x' => new stdClass()]));
}

// An AST partial selects the compatibility renderer even for a source template.
var_dump($mustache->render($source, [], ['x' => $emptyAST]));

restore_error_handler();
?>
--EXPECT--
Invalid argument
bool(false)
Object not an instance of MustacheTemplate or MustacheAST
bool(false)
Empty MustacheTemplate
bool(false)
Invalid argument
bool(false)
Empty MustacheAST
bool(false)
Empty MustacheAST
bool(false)
Partial array contains a non-string key
string(2) "AB"
Partial array contains an invalid value
string(2) "AB"
Object not an instance of MustacheTemplate or MustacheAST
string(2) "AB"
Partial array contains a non-string key
string(2) "AB"
Partial array contains an invalid value
string(2) "AB"
Object not an instance of MustacheTemplate or MustacheAST
string(2) "AB"
Empty MustacheAST
string(2) "AB"
