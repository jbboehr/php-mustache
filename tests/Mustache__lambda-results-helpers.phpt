--TEST--
Explicit results preserve helper output and use interpolation and section delimiter rules
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip mustache extension required'); ?>
--FILE--
<?php
if (!class_exists('MustacheLiteralResult') || !class_exists('MustacheTemplateResult')) die("missing explicit lambda results\n");
$m = new Mustache();
foreach ([Mustache::LAMBDA_STRING_TEMPLATE, Mustache::LAMBDA_STRING_LITERAL] as $mode) {
    $m->setLambdaStringMode($mode);
    $data = ['name' => '{{other}}', 'other' => 'Ada',
        'keep' => fn ($text, MustacheLambdaHelper $helper) => new MustacheLiteralResult($helper->render($text)),
        'expand' => fn ($text, MustacheLambdaHelper $helper) => new MustacheTemplateResult($helper->render($text)),
    ];
    foreach (['source' => '{{#keep}}{{name}}{{/keep}}|{{#expand}}{{name}}{{/expand}}',
        'AST' => $m->parse('{{#keep}}{{name}}{{/keep}}|{{#expand}}{{name}}{{/expand}}')] as $label => $root) {
        echo "$label/$mode:", $m->render($root, $data), "\n";
    }
}
$m->setStartSequence('<%');
$m->setStopSequence('%>');
foreach ([Mustache::LAMBDA_STRING_TEMPLATE, Mustache::LAMBDA_STRING_LITERAL] as $mode) {
    $m->setLambdaStringMode($mode);
    echo "delimiters/$mode:", $m->render('<%l%>|<%#l%>body<%/l%>|<%t%>|<%#t%>body<%/t%>', [
        'name' => 'Ada', 'l' => fn () => new MustacheLiteralResult('<%name%>/{{name}}'),
        't' => fn () => new MustacheTemplateResult('<%name%>/{{name}}'),
    ]), "\n";
}
?>
--EXPECT--
source/0:{{other}}|Ada
AST/0:{{other}}|Ada
source/1:{{other}}|Ada
AST/1:{{other}}|Ada
delimiters/0:&lt;%name%&gt;/{{name}}|<%name%>/{{name}}|&lt;%name%&gt;/Ada|Ada/{{name}}
delimiters/1:&lt;%name%&gt;/{{name}}|<%name%>/{{name}}|&lt;%name%&gt;/Ada|Ada/{{name}}
