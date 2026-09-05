--TEST--
Mustache accepts initialized empty templates when parsing, rendering, and preparing partials
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
function showResult($label, $callback)
{
    try {
        echo $label, ': ', json_encode($callback()), "\n";
    } catch (Throwable $error) {
        echo get_class($error), "\n";
    }
}

$mustache = new Mustache();
$partialTemplate = $mustache->parse('A{{>empty}}B');
$emptyWrapper = new MustacheTemplate('');
$templates = [
    'string' => '',
    'wrapper' => $emptyWrapper,
    'ast' => $mustache->parse(''),
];

foreach ($templates as $label => $template) {
    showResult($label . ' render', fn() => $mustache->render($template, []));
    showResult($label . ' parse', fn() => $mustache->render($mustache->parse($template), []));
    showResult($label . ' source partial', fn() => $mustache->render('A{{>empty}}B', [], ['empty' => $template]));
    showResult($label . ' AST partial', fn() => $mustache->render($partialTemplate, [], ['empty' => $template]));
}

showResult('wrapper cast', fn() => (string) $emptyWrapper);
$replaced = new MustacheTemplate('previous source');
$replaced->__construct('');
showResult('replace with empty', fn() => $mustache->render($replaced, []));
?>
--EXPECT--
string render: ""
string parse: ""
string source partial: "AB"
string AST partial: "AB"
wrapper render: ""
wrapper parse: ""
wrapper source partial: "AB"
wrapper AST partial: "AB"
ast render: ""
ast parse: ""
ast source partial: "AB"
ast AST partial: "AB"
wrapper cast: ""
replace with empty: ""
