--TEST--
MustacheAST rejects reinitialization during render
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$ast = $mustache->parse('{{#reset}}x{{/reset}}{{tail}}');
$binary = (string) $ast;
$serializedData = $ast->__serialize();

try {
  $mustache->render($ast, array(
    'reset' => function () use ($ast, $serializedData) {
      $ast->__unserialize($serializedData);
      return '';
    },
    'tail' => 'y',
  ));
  echo "accepted\n";
} catch (ValueError $exception) {
  echo "rejected\n";
}

var_dump((string) $ast === $binary);
var_dump($mustache->render($ast, ['tail' => 'y']));
?>
--EXPECT--
rejected
bool(true)
string(1) "y"
