--TEST--
Mustache partial limits apply to the optional archive serializer
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip mustache extension not loaded');
if (!method_exists(Mustache::class, 'benchmarkSerializeArchive')
    || !method_exists(Mustache::class, 'benchmarkRenderArchive')) {
    die('skip benchmark bridge disabled');
}
?>
--FILE--
<?php
if (!method_exists(Mustache::class, 'setPartialLimits')) die("missing setPartialLimits\n");

function archiveCase(Mustache $mustache, string $label, array $partials, array $data = []): void
{
    try {
        $archive = $mustache->benchmarkSerializeArchive('{{>a}}', $partials);
        echo "$label: ", $mustache->benchmarkRenderArchive($archive, $data), "\n";
    } catch (ValueError $error) {
        $limit = str_contains($error->getMessage(), 'maxEntries')
            ? 'maxEntries'
            : (str_contains($error->getMessage(), 'maxTextBytes') ? 'maxTextBytes' : 'other');
        echo "$label: rejected $limit\n";
    }
}

class ChangingArchiveSource extends MustacheTemplate
{
    public int $reads = 0;

    public function __construct(private Mustache $mustache)
    {
        unset($this->template);
    }

    public function __get($name)
    {
        ++$this->reads;
        $this->mustache->setPartialLimits();
        return 'A';
    }
}

$mustache = new Mustache();

// One-byte name plus one-byte source exactly consumes both allowances.
$mustache->setPartialLimits(1, 2);
archiveCase($mustache, 'source exact first', ['a' => 'A']);
archiveCase($mustache, 'source exact second', ['a' => 'A']);
$mustache->setPartialLimits(1, 1);
archiveCase($mustache, 'source text over', ['a' => 'A']);
$mustache->setPartialLimits(1);
archiveCase($mustache, 'unused entry over', ['a' => 'A', 'unused' => '']);

// A source AST costs its five retained source bytes plus the map name.
$sourceAst = $mustache->parse('{{x}}');
foreach ([5, 6] as $limit) {
    $mustache->setPartialLimits(maxTextBytes: $limit);
    archiveCase($mustache, "source AST $limit", ['a' => $sourceAst], ['x' => 'ok']);
}

// Its binary form instead costs the map name plus the node data: 2 bytes.
$binaryAst = MustacheAST::fromBinary($sourceAst->toBinary());
foreach ([1, 2] as $limit) {
    $mustache->setPartialLimits(maxTextBytes: $limit);
    archiveCase($mustache, "binary AST $limit", ['a' => $binaryAst], ['x' => 'ok']);
}

// The serializer captures its limits before reading a wrapper property.
$changing = new ChangingArchiveSource($mustache);
$mustache->setPartialLimits(1, 1);
archiveCase($mustache, 'callback snapshot', ['a' => $changing]);
echo "callback reads=$changing->reads\n";
archiveCase($mustache, 'callback changed later call', ['a' => 'A']);
?>
--EXPECT--
source exact first: A
source exact second: A
source text over: rejected maxTextBytes
unused entry over: rejected maxEntries
source AST 5: rejected maxTextBytes
source AST 6: ok
binary AST 1: rejected maxTextBytes
binary AST 2: ok
callback snapshot: rejected maxTextBytes
callback reads=1
callback changed later call: A
