--TEST--
Mustache archived renders capture lambda mode before argument conversion and callbacks
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip mustache extension required');
if (!method_exists(Mustache::class, 'benchmarkRenderArchive')) die('skip benchmark bridge disabled');
?>
--FILE--
<?php
if (!method_exists(Mustache::class, 'setLambdaStringMode')) die("missing lambda string mode\n");
$m = new Mustache();
$archive = $m->benchmarkSerializeArchive('{{v}}|{{{v}}}|{{#section}}body{{/section}}', ['piece' => '{{name}}']);
$plain = ['name' => 'Ada', 'v' => fn () => '<b>{{name}}</b>', 'section' => fn () => '{{>piece}}'];
foreach ([Mustache::LAMBDA_STRING_LITERAL, Mustache::LAMBDA_STRING_TEMPLATE] as $mode) {
    $m->setLambdaStringMode($mode);
    echo "$mode:", $m->benchmarkRenderArchive($archive, $plain), "\n";
}
class ModeArchive {
    public function __construct(private $m, private $archive) {}
    public function __toString(): string {
        $this->m->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);
        return $this->archive;
    }
}
echo 'argument snapshot:', $m->benchmarkRenderArchive(new ModeArchive($m, $archive), $plain), "\n";
$m->setLambdaStringMode(Mustache::LAMBDA_STRING_TEMPLATE);
$data = $plain;
$data['v'] = function () use ($m) {
    $m->setLambdaStringMode(Mustache::LAMBDA_STRING_LITERAL);
    return '{{name}}';
};
echo 'callback snapshot:', $m->benchmarkRenderArchive($archive, $data), "\n";
echo 'later:', $m->benchmarkRenderArchive($archive, $plain), "\n";
?>
--EXPECT--
1:&lt;b&gt;{{name}}&lt;/b&gt;|<b>{{name}}</b>|{{>piece}}
0:&lt;b&gt;Ada&lt;/b&gt;|<b>Ada</b>|Ada
argument snapshot:&lt;b&gt;Ada&lt;/b&gt;|<b>Ada</b>|Ada
callback snapshot:Ada|Ada|Ada
later:&lt;b&gt;{{name}}&lt;/b&gt;|<b>{{name}}</b>|{{>piece}}
