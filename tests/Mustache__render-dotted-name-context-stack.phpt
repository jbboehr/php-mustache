--TEST--
Mustache::render() resolves dotted names component by component through nested contexts
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$source = '{{profile.name}}|{{#scope}}{{profile.name}}{{/scope}}|{{#broken}}[{{profile.name}}]{{/broken}}';
$ast = $mustache->parse($source);
$templates = array(
  'source' => $source,
  'ast' => $ast,
  'binary' => new MustacheAST($ast->toBinary()),
);
$data = array(
  'profile.name' => 'root literal',
  'profile' => array('name' => 'Root &'),
  'scope' => array('profile.name' => 'current literal'),
  'broken' => array('profile' => array()),
);

foreach ($templates as $label => $template) {
  echo $label, ':', $mustache->render($template, $data), "\n";
}
?>
--EXPECT--
source:Root &amp;|Root &amp;|[]
ast:Root &amp;|Root &amp;|[]
binary:Root &amp;|Root &amp;|[]
