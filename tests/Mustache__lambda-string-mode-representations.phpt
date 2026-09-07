--TEST--
Mustache lambda string interpretation preserves escaping across template representations
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip mustache extension required'); ?>
--FILE--
<?php
if (!method_exists(Mustache::class, 'setLambdaStringMode')) die("missing lambda string mode\n");
$m = new Mustache();
$source = '{{>piece}}';
$body = '{{v}}|{{{v}}}|{{&v}}|{{#v}}body{{/v}}';
$data = new MustacheData(['name' => 'Ada', 'v' => fn () => '<b>{{name}}</b>']);
$representations = [
    'source' => [$source, $body],
    'wrapper' => [new MustacheTemplate($source), new MustacheTemplate($body)],
    'AST-root' => [$m->parse($source), $body],
    'AST-partial' => [$source, $m->parse($body)],
    'binary-AST' => [MustacheAST::fromBinary($m->parse($source)->toBinary()),
        MustacheAST::fromBinary($m->parse($body)->toBinary())],
];
foreach ($representations as $label => [$root, $partial]) {
    foreach ([Mustache::LAMBDA_STRING_LITERAL, Mustache::LAMBDA_STRING_TEMPLATE] as $mode) {
        $m->setLambdaStringMode($mode);
        echo "$label/$mode:", $m->render($root, $data, ['piece' => $partial]), "\n";
    }
}
// Interpretation is independent of tokenizer escape settings.
$m->setEscapeByDefault(false);
$m->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);
echo 'no-escape:', $m->render('{{v}}', $data), "\n";
$m->setEscapeByDefault(true);
$m->setStartSequence('<%');
$m->setStopSequence('%>');
$data = ['name' => 'Ada', 'v' => fn () => '<%name%>/{{name}}'];
foreach ([Mustache::LAMBDA_STRING_TEMPLATE, Mustache::LAMBDA_STRING_LITERAL] as $mode) {
    $m->setLambdaStringMode($mode);
    echo "delimiters/$mode:", $m->render('<%v%>|<%#v%>body<%/v%>', $data), "\n";
}
$m = new Mustache();
$m->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);
echo 'binary:', bin2hex($m->render('{{v}}', ['v' => fn () => "a\0{{name}}"])), "\n";
echo 'empty:[', $m->render('{{v}}', ['v' => fn () => '']), "]\n";
?>
--EXPECT--
source/1:&lt;b&gt;{{name}}&lt;/b&gt;|<b>{{name}}</b>|<b>{{name}}</b>|<b>{{name}}</b>
source/0:&lt;b&gt;Ada&lt;/b&gt;|<b>Ada</b>|<b>Ada</b>|<b>Ada</b>
wrapper/1:&lt;b&gt;{{name}}&lt;/b&gt;|<b>{{name}}</b>|<b>{{name}}</b>|<b>{{name}}</b>
wrapper/0:&lt;b&gt;Ada&lt;/b&gt;|<b>Ada</b>|<b>Ada</b>|<b>Ada</b>
AST-root/1:&lt;b&gt;{{name}}&lt;/b&gt;|<b>{{name}}</b>|<b>{{name}}</b>|<b>{{name}}</b>
AST-root/0:&lt;b&gt;Ada&lt;/b&gt;|<b>Ada</b>|<b>Ada</b>|<b>Ada</b>
AST-partial/1:&lt;b&gt;{{name}}&lt;/b&gt;|<b>{{name}}</b>|<b>{{name}}</b>|<b>{{name}}</b>
AST-partial/0:&lt;b&gt;Ada&lt;/b&gt;|<b>Ada</b>|<b>Ada</b>|<b>Ada</b>
binary-AST/1:&lt;b&gt;{{name}}&lt;/b&gt;|<b>{{name}}</b>|<b>{{name}}</b>|<b>{{name}}</b>
binary-AST/0:&lt;b&gt;Ada&lt;/b&gt;|<b>Ada</b>|<b>Ada</b>|<b>Ada</b>
no-escape:<b>{{name}}</b>
delimiters/0:&lt;%name%&gt;/Ada|Ada/{{name}}
delimiters/1:&lt;%name%&gt;/{{name}}|<%name%>/{{name}}
binary:61007b7b6e616d657d7d
empty:[]
