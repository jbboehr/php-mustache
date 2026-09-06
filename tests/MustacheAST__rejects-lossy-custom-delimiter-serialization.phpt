--TEST--
MustacheAST rejects legacy serialization that would lose custom section delimiters
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$ast = $mustache->parse('{{=<% %>=}}<%#lambda%>ignored<%/lambda%>');
$data = array(
  'name' => 'Ada',
  'lambda' => function () {
    return '<%name%>';
  },
);

echo 'initial:', $mustache->render($ast, $data), "\n";

$operations = array(
  'toBinary' => function () use ($ast) {
    return $ast->toBinary();
  },
  'serialize' => function () use ($ast) {
    return serialize($ast);
  },
);

foreach ($operations as $label => $operation) {
  try {
    $operation();
    echo $label, ":accepted\n";
  } catch (MustacheException $error) {
    echo $label, ':', $error->getMessage(), "\n";
  }
  echo $label, '-recovery:', $mustache->render($ast, $data), "\n";
}
?>
--EXPECT--
initial:Ada
toBinary:Legacy serialization cannot preserve custom section delimiters
toBinary-recovery:Ada
serialize:Legacy serialization cannot preserve custom section delimiters
serialize-recovery:Ada
