--TEST--
Mustache AST partials preserve their source and parse-time settings across renderers
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class SnapshotPartialTemplate extends MustacheTemplate
{
    public $source;
    public $reads = 0;
    private $mustache;

    public function __construct($mustache, $source)
    {
        $this->mustache = $mustache;
        $this->source = $source;
        unset($this->template);
    }

    public function __get($name)
    {
        $this->reads++;
        $this->mustache->setStartSequence('<%');
        $this->mustache->setStopSequence('%>');
        $this->mustache->setEscapeByDefault(false);
        return $this->source;
    }
}

$mustache = new Mustache();
$template = new SnapshotPartialTemplate(
    $mustache,
    "<%#capture%><% name %>\0<%! keep %><%/capture%>|<%name%>|{{name}}"
);
$ast = $mustache->parse($template);
$empty = $mustache->parse('');
$template->source = 'changed';
$mustache->setStartSequence('{{');
$mustache->setStopSequence('}}');
$mustache->setEscapeByDefault(true);

$other = new Mustache();
$other->setStartSequence('[[');
$other->setStopSequence(']]');

$captured = null;
$data = array(
    'name' => 'Ada &',
    'capture' => function ($text) use (&$captured) {
        $captured = $text;
        return '';
    },
);
$renders = array(
    'same' => array($mustache, '{{>piece}}/{{>empty}}/{{>tail}}', '{{name}}'),
    'other' => array($other, '[[>piece]]/[[>empty]]/[[>tail]]', '[[name]]'),
);
foreach ($renders as $label => $render) {
    $output = $render[0]->render($render[1], $data, array(
        'piece' => $ast,
        'empty' => $empty,
        'tail' => $render[2],
    ));
    echo $label, ':', $output, "\n";
    echo $label, '-body:', str_replace("\0", '<NUL>', $captured), "\n";
}
echo 'source-reads:', $template->reads, "\n";
?>
--EXPECT--
same:|Ada &|{{name}}//Ada &amp;
same-body:<% name %><NUL><%! keep %>
other:|Ada &|{{name}}//Ada &amp;
other-body:<% name %><NUL><%! keep %>
source-reads:1
