--TEST--
Explicit callback results release temporary values on success and failure and retain no helper or engine
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip mustache extension required'); ?>
--FILE--
<?php
if (!class_exists('MustacheLiteralResult') || !class_exists('MustacheTemplateResult')) die("missing explicit lambda results\n");
$m = new Mustache();
foreach (['source' => '{{#v}}body{{/v}}', 'AST' => $m->parse('{{#v}}body{{/v}}')] as $label => $root) {
    $weak = null;
    $helper = null;
    echo "$label:", $m->render($root, ['v' => function ($text, MustacheLambdaHelper $h) use (&$weak, &$helper) {
        $value = new MustacheLiteralResult('{{unexpanded}}');
        $weak = WeakReference::create($value);
        $helper = $h;
        return $value;
    }]), "\n";
    var_dump($weak->get());
    try { $helper->render('x'); echo "helper:active\n"; }
    catch (MustacheException $e) { echo "helper:expired\n"; }
    try {
        $m->render($root, ['v' => function () use (&$weak) {
            $value = new MustacheTemplateResult('{{#unclosed}}');
            $weak = WeakReference::create($value);
            return $value;
        }]);
        echo "parser:accepted\n";
    } catch (MustacheParserException $e) { echo "parser:rejected\n"; }
    var_dump($weak->get());
    $error = new RuntimeException('original');
    try { $m->render($root, ['v' => function () use ($error) { throw $error; }]); }
    catch (RuntimeException $e) { var_dump($e === $error); }
    echo $m->render($root, ['v' => fn () => new MustacheTemplateResult('recovered')]), "\n";
}
$weakHelper = null;
$saved = null;
$m->render('{{#v}}text{{/v}}', ['v' => function ($text, MustacheLambdaHelper $h) use (&$weakHelper, &$saved) {
    $weakHelper = WeakReference::create($h);
    return $saved = new MustacheLiteralResult($h->render($text));
}]);
$weakEngine = WeakReference::create($m);
unset($m);
var_dump($weakHelper->get(), $weakEngine->get());
echo $saved->getText(), "\n";
$weakResult = WeakReference::create($saved);
$data = new MustacheData(['v' => fn () => $saved]);
unset($saved);
$m = new Mustache();
echo $m->render('{{v}}|{{v}}', $data), "\n";
unset($data);
var_dump($weakResult->get());
?>
--EXPECT--
source:{{unexpanded}}
NULL
helper:expired
parser:rejected
NULL
bool(true)
recovered
AST:{{unexpanded}}
NULL
helper:expired
parser:rejected
NULL
bool(true)
recovered
NULL
NULL
text
text|text
NULL
