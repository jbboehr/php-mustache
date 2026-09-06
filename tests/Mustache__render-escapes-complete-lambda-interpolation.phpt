--TEST--
Mustache::render() escapes the complete evaluated result of an interpolation lambda
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$source = '{{lambda}}|{{{lambda}}}';
$ast = $mustache->parse($source);
$templates = array(
  'source' => $source,
  'ast' => $ast,
  'binary' => new MustacheAST($ast->toBinary()),
);
$data = array(
  'name' => 'Ada & Lin',
  'lambda' => function () {
    return 'literal & {{>piece}}';
  },
);
$partials = array('piece' => '<{{name}}>');

foreach ($templates as $label => $template) {
  echo $label, ':', $mustache->render($template, $data, $partials), "\n";
}

$partials['piece'] = $mustache->parse($partials['piece']);
echo 'ast-partial:', $mustache->render($source, $data, $partials), "\n";
?>
--EXPECT--
source:literal &amp; &lt;Ada &amp;amp; Lin&gt;|literal & <Ada &amp; Lin>
ast:literal &amp; &lt;Ada &amp;amp; Lin&gt;|literal & <Ada &amp; Lin>
binary:literal &amp; &lt;Ada &amp;amp; Lin&gt;|literal & <Ada &amp; Lin>
ast-partial:literal &amp; &lt;Ada &amp;amp; Lin&gt;|literal & <Ada &amp; Lin>
