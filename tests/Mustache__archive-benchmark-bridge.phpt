--TEST--
Mustache archived-template benchmark bridge
--SKIPIF--
<?php
if (!extension_loaded('mustache')) {
    die('skip mustache extension not loaded');
}
if (!method_exists(Mustache::class, 'benchmarkSerializeArchive')
    && !method_exists(Mustache::class, 'benchmarkRenderArchive')) {
    die('skip benchmark bridge disabled');
}
?>
--FILE--
<?php
$mustache = new Mustache();
$template = "\0{{>card}}";
$partials = ['card' => 'Hello {{name}}!'];
$data = ['name' => 'Ada'];
$archive = $mustache->benchmarkSerializeArchive($template, $partials);
$fromSource = $mustache->render($template, $data, $partials);
$fromArchive = $mustache->benchmarkRenderArchive($archive, $data);

var_dump(is_string($archive));
var_dump(strlen($archive) > 0);
var_dump($fromSource === $fromArchive);
echo bin2hex($fromArchive), "\n";

$roundTrip = unserialize(serialize($archive));
var_dump($roundTrip === $archive);
var_dump($mustache->benchmarkRenderArchive($roundTrip, $data) === $fromSource);

$nestedTemplate = '{{>layout}}';
$nestedPartials = [
    'layout' => '<{{>card}}>',
    'card' => '{{>badge}} {{name}}',
    'badge' => '[{{label}}]',
];
$nestedData = ['name' => 'Ada', 'label' => 'vip'];
$nestedArchive = $mustache->benchmarkSerializeArchive($nestedTemplate, $nestedPartials);
var_dump(
    $mustache->benchmarkRenderArchive($nestedArchive, $nestedData)
    === $mustache->render($nestedTemplate, $nestedData, $nestedPartials)
);

$configured = new Mustache();
$configured->setStartSequence('<%');
$configured->setStopSequence('%>');
$configuredTemplate = '<%>layout%>';
$configuredPartials = [
    'layout' => '[<%>card%>]',
    'card' => '<%name%>',
];
$configuredArchive = $configured->benchmarkSerializeArchive(
    $configuredTemplate,
    $configuredPartials,
);
$configuredFromArchive = $configured->benchmarkRenderArchive($configuredArchive, $data);
var_dump(
    $configuredFromArchive
    === $configured->render($configuredTemplate, $data, $configuredPartials)
);
var_dump($configuredFromArchive);

try {
    $mustache->benchmarkRenderArchive(substr($archive, 0, -1), []);
} catch (MustacheException $exception) {
    echo get_class($exception), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
0048656c6c6f2041646121
bool(true)
bool(true)
bool(true)
bool(true)
string(5) "[Ada]"
MustacheException
