--TEST--
Mustache rejects excessive template nesting and remains reusable
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
function nestedTemplate($depth) {
  $template = 'x';
  for ($i = 0; $i < $depth; ++$i) {
    $template = '{{#value}}' . $template . '{{/value}}';
  }
  return $template;
}

$mustache = new Mustache();
// Sixty-two sections are libmustache's tokenizer maximum and reach the
// extension's 64-node serialization and clone boundaries once root/output nodes are included.
$valid = $mustache->parse(nestedTemplate(62));
var_dump($mustache->render(
  '{{>nested}}',
  array('value' => true),
  array('nested' => $valid)
));

try {
  $mustache->parse(nestedTemplate(63));
  echo "accepted\n";
} catch (MustacheParserException $e) {
  echo $e->getMessage(), "\n";
}

var_dump($mustache->render('{{value}}', array('value' => 'reused')));
?>
--EXPECT--
string(1) "x"
Template nesting limit exceeded
string(6) "reused"
