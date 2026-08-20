--TEST--
MustacheAST rejects reinitialization during render
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$ast = $mustache->parse('{{#reset}}x{{/reset}}{{tail}}');
$binary = (string) $ast;
serialize($ast);

$warning = null;
set_error_handler(function ($level, $message) use (&$warning) {
  $warning = $message;
  return true;
});
$result = $mustache->render($ast, array(
  'reset' => function () use ($ast) {
    $ast->__wakeup();
    return '';
  },
  'tail' => 'y',
));
restore_error_handler();

var_dump(strpos($warning, 'MustacheAST is already initialized') !== false);
var_dump($result);
var_dump((string) $ast === $binary);
?>
--EXPECT--
bool(true)
string(1) "y"
bool(true)
