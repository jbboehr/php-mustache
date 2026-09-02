--TEST--
Mustache archived-template benchmark bridge enforces limits and rejects mutated archives
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
function requireArchiveFailure(callable $operation, string $case, ?string $message = null): void
{
    try {
        $operation();
    } catch (MustacheException $exception) {
        if ($message !== null && !str_contains($exception->getMessage(), $message)) {
            throw new RuntimeException("$case reported an unexpected message: {$exception->getMessage()}");
        }
        return;
    }

    throw new RuntimeException("$case was accepted");
}

$mustache = new Mustache();
$archive = $mustache->benchmarkSerializeArchive(
    'Hello {{name}}',
    ['unused' => 'partial'],
);

for ($length = 0; $length < strlen($archive); ++$length) {
    requireArchiveFailure(
        fn () => $mustache->benchmarkRenderArchive(substr($archive, 0, $length), []),
        "truncated archive at $length",
    );
}

for ($offset = 0; $offset < strlen($archive); ++$offset) {
    $mutated = $archive;
    $mutated[$offset] = chr(ord($mutated[$offset]) ^ 1);
    requireArchiveFailure(
        fn () => $mustache->benchmarkRenderArchive($mutated, []),
        "one-bit mutation at $offset",
    );
}

requireArchiveFailure(
    fn () => $mustache->benchmarkRenderArchive($archive . "\0", []),
    'archive with trailing data',
);
requireArchiveFailure(
    fn () => $mustache->benchmarkRenderArchive($archive . $archive, []),
    'concatenated archives',
);

requireArchiveFailure(
    fn () => $mustache->benchmarkRenderArchive(str_repeat("\0", 16 * 1024 * 1024 + 1), []),
    'archive byte limit',
    'input byte limit exceeded',
);
requireArchiveFailure(
    fn () => $mustache->benchmarkSerializeArchive(str_repeat('x', 16 * 1024 * 1024)),
    'archive output byte limit',
    'output byte limit exceeded',
);

$part256 = '{{' . implode('.', array_fill(0, 256, 'a')) . '}}';
$part257 = '{{' . implode('.', array_fill(0, 257, 'a')) . '}}';
$part256Archive = $mustache->benchmarkSerializeArchive($part256);
var_dump($mustache->benchmarkRenderArchive($part256Archive, []) === '');
requireArchiveFailure(
    fn () => $mustache->benchmarkSerializeArchive($part257),
    'per-node data-part limit',
    'per-node data-part limit exceeded',
);

$atAggregateLimit = str_repeat($part256, 390)
    . '{{' . implode('.', array_fill(0, 160, 'a')) . '}}';
$aggregateArchive = $mustache->benchmarkSerializeArchive($atAggregateLimit);
var_dump($mustache->benchmarkRenderArchive($aggregateArchive, []) === '');
requireArchiveFailure(
    fn () => $mustache->benchmarkSerializeArchive($atAggregateLimit . '{{a}}'),
    'aggregate data-part limit',
    'data-part limit exceeded',
);

var_dump($mustache->benchmarkRenderArchive($archive, ['name' => 'Ada']));
?>
--EXPECT--
bool(true)
bool(true)
string(9) "Hello Ada"
