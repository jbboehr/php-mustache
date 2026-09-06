--TEST--
MustacheAST rejects lossy section-body serialization and preserves canonical binaries
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$data = array(
  'name' => 'Ada',
  'lambda' => function ($text) {
    return $text;
  },
);

$lossy = $mustache->parse('{{#lambda}}A {{ name }} B{{/lambda}}');
echo 'lossy-initial:', $mustache->render($lossy, $data), "\n";

$operations = array(
  'toBinary' => function () use ($lossy) {
    return $lossy->toBinary();
  },
  'serialize' => function () use ($lossy) {
    return serialize($lossy);
  },
);
foreach ($operations as $label => $operation) {
  try {
    $operation();
    echo $label, ":accepted\n";
  } catch (MustacheException $error) {
    echo $label, ':', $error->getMessage(), "\n";
  }
  echo $label, '-recovery:', $mustache->render($lossy, $data), "\n";
}

$canonical = $mustache->parse('{{#lambda}}A {{name}} B{{/lambda}}');
$binaryCopy = new MustacheAST($canonical->toBinary());
$serializedCopy = unserialize(serialize($canonical));
echo 'binary-copy:', $mustache->render($binaryCopy, $data), "\n";
echo 'serialized-copy:', $mustache->render($serializedCopy, $data), "\n";
?>
--EXPECT--
lossy-initial:A Ada B
toBinary:Legacy serialization cannot preserve original section text
toBinary-recovery:A Ada B
serialize:Legacy serialization cannot preserve original section text
serialize-recovery:A Ada B
binary-copy:A Ada B
serialized-copy:A Ada B
