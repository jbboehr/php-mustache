--TEST--
Mustache::render() produces equivalent output for source and AST partials
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$mustache->setStartSequence('<%');
$mustache->setStopSequence('%>');

$template = "Start\n  <%>outer%>\nEnd\n";
$partials = array(
  'outer' => "<%#decorate%><%value%><%/decorate%>\n<%>inner%>",
  'inner' => "inner: <%value%>\n",
);
$data = array(
  'value' => 'x',
  'decorate' => function ($text, MustacheLambdaHelper $helper) {
    return strtoupper($helper->render('{{value}}'));
  },
);

$compiled = $mustache->render($template, $data, $partials);
$partials['outer'] = $mustache->parse($partials['outer']);
$compatible = $mustache->render($template, $data, $partials);

var_dump($compiled === $compatible);
var_dump($compatible);
?>
--EXPECT--
bool(true)
string(25) "Start
  X
  inner: x
End
"
