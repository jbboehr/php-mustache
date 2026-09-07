--TEST--
Mustache archived templates preserve PHP lambda evaluation, escaping, and context
--SKIPIF--
<?php
if (!extension_loaded('mustache')) {
    die('skip mustache extension not loaded');
}
if (!method_exists(Mustache::class, 'benchmarkSerializeArchive')
    || !method_exists(Mustache::class, 'benchmarkRenderArchive')) {
    die('skip benchmark bridge disabled');
}
?>
--FILE--
<?php
$mustache = new Mustache();
$template = '{{#outer}}{{value}}|{{{value}}}|{{#section}}A {{name}} B{{/section}}{{/outer}}';
$partials = ['piece' => '[{{name}}]'];
$archive = $mustache->benchmarkSerializeArchive($template, $partials);
$transportedArchive = unserialize(serialize($archive));

$nestedOutput = null;
$retainedHelper = null;
$sectionCalls = 0;
$data = [
    'outer' => [
        'name' => 'A&B',
        'value' => fn () => '<b>{{name}}|{{>piece}}</b>',
        'section' => function ($text, MustacheLambdaHelper $helper) use (
            &$nestedOutput,
            &$retainedHelper,
            &$sectionCalls,
        ) {
            ++$sectionCalls;
            $nestedOutput = $helper->render('{{name}}|{{>piece}}');
            $retainedHelper = $helper;
            return $helper->render($text) . '|<i>{{name}}</i>|{{>piece}}';
        },
    ],
];

$renders = [
    'source' => fn () => $mustache->render($template, $data, $partials),
    'archive' => fn () => $mustache->benchmarkRenderArchive($archive, $data),
    'transported' => fn () => $mustache->benchmarkRenderArchive($transportedArchive, $data),
];

foreach ($renders as $label => $render) {
    echo $label, ':', $render(), "\n";
    echo 'callback:', $nestedOutput, ':', $sectionCalls, "\n";
    try {
        $retainedHelper->render('{{name}}');
        echo "retained-helper-active\n";
    } catch (MustacheException $exception) {
        echo "retained-helper-inactive\n";
    }
}

var_dump($transportedArchive === $archive);
?>
--EXPECT--
source:&lt;b&gt;A&amp;amp;B|[A&amp;amp;B]&lt;/b&gt;|<b>A&amp;B|[A&amp;B]</b>|A A&amp;B B|<i>A&amp;B</i>|[A&amp;B]
callback:A&amp;B|[A&amp;B]:1
retained-helper-inactive
archive:&lt;b&gt;A&amp;amp;B|[A&amp;amp;B]&lt;/b&gt;|<b>A&amp;B|[A&amp;B]</b>|A A&amp;B B|<i>A&amp;B</i>|[A&amp;B]
callback:A&amp;B|[A&amp;B]:2
retained-helper-inactive
transported:&lt;b&gt;A&amp;amp;B|[A&amp;amp;B]&lt;/b&gt;|<b>A&amp;B|[A&amp;B]</b>|A A&amp;B B|<i>A&amp;B</i>|[A&amp;B]
callback:A&amp;B|[A&amp;B]:3
retained-helper-inactive
bool(true)
