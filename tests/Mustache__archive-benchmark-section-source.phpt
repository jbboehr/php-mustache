--TEST--
Mustache archives preserve original section callback text and template-return behavior
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip mustache extension not loaded');
if (!method_exists(Mustache::class, 'benchmarkSerializeArchive')) {
    die('skip benchmark bridge disabled');
}
?>
--FILE--
<?php
$cases = [
    'spelling' => ['{{', '}}', "A {{ name }} {{! keep }} {{{name}}}\r\nB"],
    'nul' => ['{{', '}}', "A\0{{ name }}\0B"],
    'delimiters' => ['<%', '%>', 'A <% name %> <%! keep %> <%&name%> B'],
];

foreach ($cases as $label => [$start, $stop, $body]) {
    foreach (['root', 'source-partial', 'AST-partial'] as $representation) {
        $mustache = new Mustache();
        $mustache->setStartSequence($start);
        $mustache->setStopSequence($stop);
        $section = $start . '#capture' . $stop . $body . $start . '/capture' . $stop;
        $partials = [];
        if ($representation === 'root') {
            $root = '[' . $section . ']';
        } else {
            $root = '[' . $start . '>piece' . $stop . ']';
            $partials['piece'] = $representation === 'AST-partial'
                ? $mustache->parse($section) : $section;
        }

        $archive = $mustache->benchmarkSerializeArchive($root, $partials);
        $captured = null;
        $calls = 0;
        $output = $mustache->benchmarkRenderArchive($archive, [
            'name' => 'Ada',
            'capture' => function ($text) use (&$captured, &$calls, $start, $stop) {
                $captured = $text;
                ++$calls;
                return '<b>' . $start . 'name' . $stop . '</b>';
            },
        ]);
        echo $label, '/', $representation, ':', $captured === $body ? 'exact' : 'changed',
            ':', $calls, ':', $output, "\n";
    }
}
?>
--EXPECT--
spelling/root:exact:1:[<b>Ada</b>]
spelling/source-partial:exact:1:[<b>Ada</b>]
spelling/AST-partial:exact:1:[<b>Ada</b>]
nul/root:exact:1:[<b>Ada</b>]
nul/source-partial:exact:1:[<b>Ada</b>]
nul/AST-partial:exact:1:[<b>Ada</b>]
delimiters/root:exact:1:[<b>Ada</b>]
delimiters/source-partial:exact:1:[<b>Ada</b>]
delimiters/AST-partial:exact:1:[<b>Ada</b>]
