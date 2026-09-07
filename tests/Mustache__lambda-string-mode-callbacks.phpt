--TEST--
Mustache captures lambda mode before wrapper reads and preserves it through nested callbacks
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip mustache extension required'); ?>
--FILE--
<?php
if (!method_exists(Mustache::class, 'setLambdaStringMode')) die("missing lambda string mode\n");
class ModeSource extends MustacheTemplate {
    public function __construct(private $source, private $change) { unset($this->template); }
    public function __get($name) { ($this->change)(); return $this->source; }
}
$m = new Mustache();
$plain = ['name' => 'Ada', 'v' => fn () => '{{name}}'];
foreach (['source' => '{{>piece}}', 'AST' => $m->parse('{{>piece}}')] as $label => $root) {
    $m->setLambdaStringMode(Mustache::LAMBDA_STRING_TEMPLATE);
    $partial = new ModeSource('{{v}}', function () use ($m, $plain) {
        $m->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);
        echo 'nested:', $m->render('{{v}}', $plain), "\n";
    });
    $output = $m->render($root, $plain, ['piece' => $partial]);
    echo "$label getter:", $output, "\n";
    echo 'later:', $m->render('{{v}}', $plain), "\n";
}
$m->setLambdaStringMode(Mustache::LAMBDA_STRING_TEMPLATE);
$root = new ModeSource('{{v}}', fn () => $m->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL));
echo 'root getter:', $m->render($root, $plain), "\n";
foreach (['source' => '{{#change}}body{{/change}}|{{v}}', 'AST' => $m->parse('{{#change}}body{{/change}}|{{v}}')] as $label => $root) {
    $m->setLambdaStringMode(Mustache::LAMBDA_STRING_TEMPLATE);
    $saved = null;
    $data = $plain + ['change' => function ($text, MustacheLambdaHelper $helper) use ($m, $plain, &$saved) {
        $m->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);
        $saved = $helper;
        return $helper->render('{{v}}') . '/' . $m->render('{{v}}', $plain);
    }];
    echo "$label callback:", $m->render($root, $data), ':configured=', $m->getLambdaStringMode(), "\n";
    try { $saved->render('text'); } catch (MustacheException $e) { echo "helper inactive\n"; }
    $failure = new RuntimeException('callback failed');
    try {
        $m->render($root, $plain + ['change' => function () use ($m, $failure) {
            $m->setLambdaStringMode(Mustache::LAMBDA_STRING_TEMPLATE);
            throw $failure;
        }]);
    } catch (RuntimeException $e) { echo 'same exception:', $e === $failure ? 'yes' : 'no', "\n"; }
    echo 'recovered:', $m->render($root, $plain + ['change' => fn () => '{{name}}']), "\n";
}
?>
--EXPECT--
nested:{{name}}
source getter:Ada
later:{{name}}
nested:{{name}}
AST getter:Ada
later:{{name}}
root getter:Ada
source callback:Ada/Ada|Ada:configured=1
helper inactive
same exception:yes
recovered:Ada|Ada
AST callback:Ada/Ada|Ada:configured=1
helper inactive
same exception:yes
recovered:Ada|Ada
