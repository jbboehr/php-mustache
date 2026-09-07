--TEST--
Explicit results work with closure, invokable, method, and reference returns while preserving legacy conversions
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip mustache extension required'); ?>
--FILE--
<?php
if (!class_exists('MustacheLiteralResult') || !class_exists('MustacheTemplateResult')) die("missing explicit lambda results\n");
class ResultMethods {
    private $literal;
    private $template;
    public $name = 'Ada';
    public function __construct() {
        $this->literal = new MustacheLiteralResult('{{name}}');
        $this->template = new MustacheTemplateResult('{{name}}');
    }
    public function &literal() { return $this->literal; }
    public function &template() { return $this->template; }
}
class ResultInvoker {
    public function __construct(private $result) {}
    public function &__invoke() { return $this->result; }
}
class LegacyTemplate extends MustacheTemplate {
    public function __toString(): string { return 'converted {{name}}'; }
}
$m = new Mustache();
$literal = new MustacheLiteralResult('{{name}}');
$template = new MustacheTemplateResult('{{name}}');
$closureLiteral = function &() use (&$literal) { return $literal; };
$closureTemplate = function &() use (&$template) { return $template; };
$body = '{{literal}}|{{template}}|{{#literal}}body{{/literal}}|{{#template}}body{{/template}}';
foreach ([Mustache::LAMBDA_STRING_TEMPLATE, Mustache::LAMBDA_STRING_LITERAL] as $mode) {
    $m->setLambdaStringMode($mode);
    echo "closure/$mode:", $m->render($body, ['name' => 'Ada', 'literal' => $closureLiteral, 'template' => $closureTemplate]), "\n";
    echo "invokable/$mode:", $m->render($body, ['name' => 'Ada', 'literal' => new ResultInvoker($literal), 'template' => new ResultInvoker($template)]), "\n";
    echo "method/$mode:", $m->render($body, new ResultMethods()), "\n";
    $legacy = new LegacyTemplate('stored {{name}}');
    echo "legacy/$mode:", $m->render('{{plain}}|{{wrapper}}|{{null}}|{{false}}|{{true}}|{{int}}|{{float}}', [
        'name' => 'Ada', 'plain' => fn () => '{{name}}', 'wrapper' => fn () => $legacy,
        'null' => fn () => null, 'false' => fn () => false, 'true' => fn () => true,
        'int' => fn () => 42, 'float' => fn () => 1.5,
    ]), "\n";
}
// Reference returns must not replace the caller's object with its text.
var_dump($literal instanceof MustacheLiteralResult, $template instanceof MustacheTemplateResult);
echo $literal->getText(), '|', $template->getText(), "\n";
?>
--EXPECT--
closure/0:{{name}}|Ada|{{name}}|Ada
invokable/0:{{name}}|Ada|{{name}}|Ada
method/0:{{name}}|Ada|{{name}}|Ada
legacy/0:Ada|converted Ada|||1|42|1.5
closure/1:{{name}}|Ada|{{name}}|Ada
invokable/1:{{name}}|Ada|{{name}}|Ada
method/1:{{name}}|Ada|{{name}}|Ada
legacy/1:{{name}}|converted {{name}}|||1|42|1.5
bool(true)
bool(true)
{{name}}|{{name}}
