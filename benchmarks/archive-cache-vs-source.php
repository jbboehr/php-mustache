<?php

declare(strict_types=1);

const TARGETS = [
    'small' => 1024,
    'medium' => 32768,
    'large' => 262144,
];

const MATERIAL_WIN_PERCENT = 20.0;

const MATERIAL_WIN_WORKLOADS = [
    'medium-flat',
    'medium-graph',
    'large-flat',
    'large-graph',
];

function buildWorkload(string $name, int $targetBytes, bool $nestedPartials): array
{
    $root = <<<'MUSTACHE'
<!doctype html>
<html>
<body>
{{#products}}
  {{> layout}}
{{/products}}
</body>
</html>

MUSTACHE;
    $layout = <<<'MUSTACHE'
<main aria-label="{{title}}">
  {{> card}}
</main>

MUSTACHE;
    $badge = <<<'MUSTACHE'
{{#featured}}<strong class="badge">Featured</strong>{{/featured}}

MUSTACHE;
    $unit = <<<'MUSTACHE'
{{! production-shaped product card }}
<section class="product product--{{category.slug}}">
  <h2>{{title}}</h2>
  {{#available}}
    <p>{{description}}</p>
    <span data-id="{{id}}">{{price}}</span>
  {{/available}}
  {{^available}}
    <span class="unavailable">Unavailable</span>
  {{/available}}
  {{> badge}}
</section>

MUSTACHE;

    if (!$nestedPartials) {
        $unit = str_replace(
            "  {{> badge}}\n",
            "  {{#featured}}<strong class=\"badge\">Featured</strong>{{/featured}}\n",
            $unit,
        );
        $prefix = "<!doctype html>\n<html>\n<body>\n{{#products}}\n";
        $suffix = "{{/products}}\n</body>\n</html>\n";
        $body = '';
        while (strlen($prefix) + strlen($body) + strlen($suffix) < $targetBytes) {
            $body .= $unit;
        }

        return [
            'name' => $name,
            'target_source_bytes' => $targetBytes,
            'root' => $prefix . $body . $suffix,
            'partials' => [],
        ];
    }

    $card = '';
    while (strlen($root) + strlen($layout) + strlen($badge) + strlen($card) < $targetBytes) {
        $card .= $unit;
    }

    return [
        'name' => $name,
        'target_source_bytes' => $targetBytes,
        'root' => $root,
        'partials' => [
            'layout' => $layout,
            'card' => $card,
            'badge' => $badge,
        ],
    ];
}

function graphBytes(array $graph): int
{
    return strlen($graph['root']) + array_sum(array_map('strlen', $graph['partials']));
}

function benchmarkData(): array
{
    return [
        'products' => [[
            'id' => 'product-123',
            'title' => 'A rock-hard template engine',
            'description' => 'Safe ownership, bounded parsing, and compatible rendering.',
            'price' => '$19.50',
            'available' => true,
            'featured' => true,
            'category' => ['slug' => 'libraries'],
        ]],
    ];
}

function consume(mixed $value): int
{
    if (is_string($value)) {
        return strlen($value);
    }
    if (is_int($value)) {
        return $value;
    }
    if (is_bool($value)) {
        return $value ? 1 : 0;
    }
    if (is_array($value)) {
        return count($value);
    }
    return 0;
}

function percentile(array $sorted, float $fraction): float
{
    $index = max(0, (int) ceil(count($sorted) * $fraction) - 1);
    return $sorted[$index];
}

function measure(callable $operation, int $warmups, int $samples): array
{
    $sink = 0;
    for ($i = 0; $i < $warmups; ++$i) {
        $sink ^= consume($operation());
    }

    gc_collect_cycles();
    if (function_exists('memory_reset_peak_usage')) {
        memory_reset_peak_usage();
    }
    $memoryBefore = memory_get_usage(false);
    $sink ^= consume($operation());
    $phpPeakDelta = max(0, memory_get_peak_usage(false) - $memoryBefore);

    $durations = [];
    gc_collect_cycles();
    $gcWasEnabled = gc_enabled();
    if ($gcWasEnabled) {
        gc_disable();
    }
    try {
        for ($i = 0; $i < $samples; ++$i) {
            $start = hrtime(true);
            $value = $operation();
            $durations[] = (float) (hrtime(true) - $start);
            $sink ^= consume($value);
            unset($value);
        }
    } finally {
        if ($gcWasEnabled) {
            gc_enable();
        }
    }

    sort($durations, SORT_NUMERIC);
    $median = percentile($durations, 0.50);
    $p95 = percentile($durations, 0.95);

    return [
        'median_us' => $median / 1000.0,
        'p95_us' => $p95 / 1000.0,
        'min_us' => $durations[0] / 1000.0,
        'max_us' => $durations[count($durations) - 1] / 1000.0,
        'throughput_per_second' => 1_000_000_000.0 / $median,
        'php_peak_delta_bytes' => $phpPeakDelta,
        'sink' => $sink,
    ];
}

function extensionPath(string $name): string
{
    $setting = 'BENCH_' . strtoupper($name) . '_EXTENSION';
    $path = getenv($setting);
    if (!is_string($path) || $path === '' || !is_file($path)) {
        throw new RuntimeException("{$setting} must name the loaded {$name} module");
    }
    return $path;
}

function coldWorkerCommand(): array
{
    return [
        PHP_BINARY,
        '-n',
        '-d',
        'extension=' . extensionPath('apcu'),
        '-d',
        'apc.enable_cli=1',
        '-d',
        'extension=' . extensionPath('mustache'),
        __FILE__,
        '--cold-sample',
    ];
}

function writeColdFixture(array $sourceGraph, string $archive, array $data, string $expectedOutput): string
{
    $path = tempnam(sys_get_temp_dir(), 'mustache-archive-bench-');
    if (!is_string($path)) {
        throw new RuntimeException('could not allocate a cold-worker fixture');
    }

    $fixture = serialize([
        'source_graph' => $sourceGraph,
        'archive' => $archive,
        'source_php_payload' => serialize($sourceGraph),
        'archive_php_payload' => serialize($archive),
        'data' => $data,
        'expected_output_hash' => hash('sha256', $expectedOutput),
        'expected_output_bytes' => strlen($expectedOutput),
    ]);
    if (file_put_contents($path, $fixture) !== strlen($fixture)) {
        @unlink($path);
        throw new RuntimeException('could not write a complete cold-worker fixture');
    }
    return $path;
}

function coldWorkerSample(string $fixturePath, string $case): void
{
    $contents = file_get_contents($fixturePath);
    if (!is_string($contents)) {
        throw new RuntimeException('could not read the cold-worker fixture');
    }
    $fixture = unserialize($contents, ['allowed_classes' => false]);
    if (!is_array($fixture)) {
        throw new RuntimeException('invalid cold-worker fixture');
    }

    $isArchive = str_ends_with($case, '_archive');
    $useApcu = str_starts_with($case, 'cold_apcu_');
    if (!$isArchive && !str_ends_with($case, '_source')) {
        throw new RuntimeException("unknown cold-worker case {$case}");
    }

    $cacheKey = 'mustache-archive-bench-cold-' . getmypid();
    if ($useApcu) {
        $cached = $isArchive ? $fixture['archive'] : $fixture['source_graph'];
        if (!apcu_store($cacheKey, $cached)) {
            throw new RuntimeException('could not prime APCu in the cold worker');
        }
    }

    $mustache = new Mustache();
    gc_collect_cycles();
    gc_disable();
    if (function_exists('memory_reset_peak_usage')) {
        memory_reset_peak_usage();
    }
    $memoryBefore = memory_get_usage(false);
    $start = hrtime(true);
    if ($useApcu) {
        $value = apcu_fetch($cacheKey);
    } else {
        $payload = $isArchive ? $fixture['archive_php_payload'] : $fixture['source_php_payload'];
        $value = unserialize($payload, ['allowed_classes' => false]);
    }
    $output = $isArchive
        ? $mustache->benchmarkRenderArchive($value, $fixture['data'])
        : $mustache->render($value['root'], $fixture['data'], $value['partials']);
    $elapsed = hrtime(true) - $start;
    $phpPeakDelta = max(0, memory_get_peak_usage(false) - $memoryBefore);

    if (strlen($output) !== $fixture['expected_output_bytes']
        || hash('sha256', $output) !== $fixture['expected_output_hash']) {
        throw new RuntimeException("cold-worker rendering differs for {$case}");
    }
    if ($useApcu) {
        apcu_delete($cacheKey);
    }

    echo json_encode([
        'elapsed_ns' => $elapsed,
        'php_peak_delta_bytes' => $phpPeakDelta,
        'rendered_bytes' => strlen($output),
    ], JSON_THROW_ON_ERROR);
}

function runColdWorkerSample(array $command, string $fixturePath, string $case): array
{
    $descriptors = [
        0 => ['pipe', 'r'],
        1 => ['pipe', 'w'],
        2 => ['pipe', 'w'],
    ];
    $process = proc_open([...$command, $fixturePath, $case], $descriptors, $pipes);
    if (!is_resource($process)) {
        throw new RuntimeException('could not start a cold-worker sample');
    }
    fclose($pipes[0]);
    $stdout = stream_get_contents($pipes[1]);
    fclose($pipes[1]);
    $stderr = stream_get_contents($pipes[2]);
    fclose($pipes[2]);
    $status = proc_close($process);
    if ($status !== 0 || !is_string($stdout)) {
        throw new RuntimeException("cold-worker sample failed ({$status}): " . trim((string) $stderr));
    }

    $result = json_decode($stdout, true, flags: JSON_THROW_ON_ERROR);
    if (!is_array($result) || !is_int($result['elapsed_ns']) || !is_int($result['php_peak_delta_bytes'])) {
        throw new RuntimeException('cold-worker sample returned invalid measurements');
    }
    return $result;
}

function measureColdWorkers(array $command, string $fixturePath, string $case, int $samples): array
{
    $durations = [];
    $peakDeltas = [];
    $sink = 0;
    for ($i = 0; $i < $samples; ++$i) {
        $sample = runColdWorkerSample($command, $fixturePath, $case);
        $durations[] = (float) $sample['elapsed_ns'];
        $peakDeltas[] = $sample['php_peak_delta_bytes'];
        $sink ^= $sample['rendered_bytes'];
    }

    sort($durations, SORT_NUMERIC);
    sort($peakDeltas, SORT_NUMERIC);
    $median = percentile($durations, 0.50);
    $p95 = percentile($durations, 0.95);
    return [
        'median_us' => $median / 1000.0,
        'p95_us' => $p95 / 1000.0,
        'min_us' => $durations[0] / 1000.0,
        'max_us' => $durations[count($durations) - 1] / 1000.0,
        'throughput_per_second' => 1_000_000_000.0 / $median,
        'php_peak_delta_bytes' => (int) percentile($peakDeltas, 0.50),
        'sink' => $sink,
    ];
}

function printResult(string $workload, string $case, array $result): void
{
    printf(
        "%-13s %-25s %11.3f %11.3f %11.0f %12d\n",
        $workload,
        $case,
        $result['median_us'],
        $result['p95_us'],
        $result['throughput_per_second'],
        $result['php_peak_delta_bytes'],
    );
}

function improvement(float $baseline, float $candidate): float
{
    return (($baseline - $candidate) / $baseline) * 100.0;
}

if (!extension_loaded('mustache') || !extension_loaded('apcu')) {
    fwrite(STDERR, "the benchmark requires mustache and APCu\n");
    exit(2);
}
if (!method_exists(Mustache::class, 'benchmarkSerializeArchive')) {
    fwrite(STDERR, "mustache was not built with --enable-mustache-archive-benchmark\n");
    exit(2);
}
$apcuCliSetting = strtolower((string) ini_get('apc.enable_cli'));
if (!in_array($apcuCliSetting, ['1', 'on', 'true', 'yes'], true)) {
    fwrite(STDERR, "the benchmark requires apc.enable_cli=1\n");
    exit(2);
}

if (($argv[1] ?? '') === '--cold-sample') {
    try {
        coldWorkerSample((string) ($argv[2] ?? ''), (string) ($argv[3] ?? ''));
        exit(0);
    } catch (Throwable $error) {
        fwrite(STDERR, $error->getMessage() . "\n");
        exit(2);
    }
}

$samples = max(21, (int) (getenv('BENCH_SAMPLES') ?: 101));
$warmups = max(3, (int) (getenv('BENCH_WARMUPS') ?: 10));
$coldSamplesSetting = getenv('BENCH_COLD_SAMPLES');
$coldSamples = $coldSamplesSetting === false ? 31 : max(0, (int) $coldSamplesSetting);
$mustache = new Mustache();
$data = benchmarkData();
$coldCommand = $coldSamples > 0 ? coldWorkerCommand() : [];

$metadata = [
    'generated_at_utc' => gmdate(DATE_ATOM),
    'php_version' => PHP_VERSION,
    'php_sapi' => PHP_SAPI,
    'mustache_extension_version' => phpversion('mustache'),
    'apcu_version' => phpversion('apcu'),
    'libmustache_revision' => getenv('BENCH_LIBMUSTACHE_REVISION') ?: null,
    'php_mustache_revision' => getenv('BENCH_PHP_MUSTACHE_REVISION') ?: null,
    'samples' => $samples,
    'warmups' => $warmups,
    'cold_samples' => $coldSamples,
    'material_win_percent' => MATERIAL_WIN_PERCENT,
    'gc_during_timed_samples' => 'disabled',
    'cold_worker_model' => 'fresh PHP CLI process; startup and fixture loading excluded; APCu primed before first timed fetch',
];
$allResults = [];

printf(
    "PHP %s, mustache %s, APCu %s, %d samples after %d warmups, %d cold workers\n",
    PHP_VERSION,
    (string) phpversion('mustache'),
    (string) phpversion('apcu'),
    $samples,
    $warmups,
    $coldSamples,
);
printf("%-13s %-25s %11s %11s %11s %12s\n", 'workload', 'case', 'median us', 'p95 us', 'ops/s', 'PHP peak B');

$workloads = [];
foreach (TARGETS as $size => $targetBytes) {
    $workloads[] = buildWorkload("{$size}-flat", $targetBytes, false);
    $workloads[] = buildWorkload("{$size}-graph", $targetBytes, true);
}

foreach ($workloads as $workload) {
    $name = $workload['name'];
    $sourceGraph = ['root' => $workload['root'], 'partials' => $workload['partials']];
    $archive = $mustache->benchmarkSerializeArchive($sourceGraph['root'], $sourceGraph['partials']);
    $sourceOutput = $mustache->render($sourceGraph['root'], $data, $sourceGraph['partials']);
    $archiveOutput = $mustache->benchmarkRenderArchive($archive, $data);
    if ($sourceOutput !== $archiveOutput || $sourceOutput === '') {
        throw new RuntimeException("source and archive rendering differ for {$name}");
    }

    $sourcePhpPayload = serialize($sourceGraph);
    $archivePhpPayload = serialize($archive);
    $sourceKey = "mustache-archive-bench-source-{$name}";
    $archiveKey = "mustache-archive-bench-archive-{$name}";
    if (!apcu_store($sourceKey, $sourceGraph) || !apcu_store($archiveKey, $archive)) {
        throw new RuntimeException("could not prime APCu for {$name}");
    }

    $cases = [
        'compile_render_source' => static fn (): string => $mustache->render(
            $sourceGraph['root'],
            $data,
            $sourceGraph['partials'],
        ),
        'load_render_archive' => static fn (): string => $mustache->benchmarkRenderArchive($archive, $data),
        'php_cache_hit_source' => static function () use ($mustache, $sourcePhpPayload, $data): string {
            $graph = unserialize($sourcePhpPayload, ['allowed_classes' => false]);
            return $mustache->render($graph['root'], $data, $graph['partials']);
        },
        'php_cache_hit_archive' => static function () use ($mustache, $archivePhpPayload, $data): string {
            $bytes = unserialize($archivePhpPayload, ['allowed_classes' => false]);
            return $mustache->benchmarkRenderArchive($bytes, $data);
        },
        'php_serialize_source' => static fn (): string => serialize($sourceGraph),
        'php_serialize_archive' => static fn (): string => serialize($archive),
        'apcu_cache_hit_source' => static function () use ($mustache, $sourceKey, $data): string {
            $graph = apcu_fetch($sourceKey);
            return $mustache->render($graph['root'], $data, $graph['partials']);
        },
        'apcu_cache_hit_archive' => static function () use ($mustache, $archiveKey, $data): string {
            return $mustache->benchmarkRenderArchive(apcu_fetch($archiveKey), $data);
        },
        'apcu_write_source' => static fn (): bool => apcu_store($sourceKey, $sourceGraph),
        'apcu_write_archive' => static function () use ($mustache, $archiveKey, $sourceGraph): bool {
            return apcu_store(
                $archiveKey,
                $mustache->benchmarkSerializeArchive($sourceGraph['root'], $sourceGraph['partials']),
            );
        },
    ];

    $workloadResults = [
        'target_source_bytes' => $workload['target_source_bytes'],
        'source_bytes' => graphBytes($sourceGraph),
        'archive_bytes' => strlen($archive),
        'archive_to_source_ratio' => strlen($archive) / graphBytes($sourceGraph),
        'php_source_payload_bytes' => strlen($sourcePhpPayload),
        'php_archive_payload_bytes' => strlen($archivePhpPayload),
        'rendered_bytes' => strlen($sourceOutput),
        'cases' => [],
    ];

    $fixturePath = writeColdFixture($sourceGraph, $archive, $data, $sourceOutput);
    try {
        foreach ($cases as $case => $operation) {
            $result = measure($operation, $warmups, $samples);
            $workloadResults['cases'][$case] = $result;
            printResult($name, $case, $result);
        }

        if ($coldSamples > 0) {
            foreach ([
                'cold_php_source',
                'cold_php_archive',
                'cold_apcu_source',
                'cold_apcu_archive',
            ] as $case) {
                $result = measureColdWorkers($coldCommand, $fixturePath, $case, $coldSamples);
                $workloadResults['cases'][$case] = $result;
                printResult($name, $case, $result);
            }
        }
    } finally {
        @unlink($fixturePath);
        apcu_delete($sourceKey);
        apcu_delete($archiveKey);
    }

    if (str_starts_with($name, 'medium-') || str_starts_with($name, 'large-')) {
        $source = $workloadResults['cases']['apcu_cache_hit_source'];
        $candidate = $workloadResults['cases']['apcu_cache_hit_archive'];
        $medianWin = improvement($source['median_us'], $candidate['median_us']);
        $p95Win = improvement($source['p95_us'], $candidate['p95_us']);
        $workloadResults['material_win'] = [
            'median_percent' => $medianWin,
            'p95_percent' => $p95Win,
            'passes' => $medianWin >= MATERIAL_WIN_PERCENT && $p95Win >= MATERIAL_WIN_PERCENT,
        ];
    }
    $allResults[$name] = $workloadResults;
}

echo "\nPayload sizes:\n";
printf(
    "%-13s %12s %12s %9s %14s %14s\n",
    'workload',
    'source B',
    'archive B',
    'ratio',
    'PHP source B',
    'PHP archive B',
);
foreach ($allResults as $name => $result) {
    printf(
        "%-13s %12d %12d %8.2fx %14d %14d\n",
        $name,
        $result['source_bytes'],
        $result['archive_bytes'],
        $result['archive_to_source_ratio'],
        $result['php_source_payload_bytes'],
        $result['php_archive_payload_bytes'],
    );
}

echo "\nAPCu cache-hit decision (requires >= " . MATERIAL_WIN_PERCENT . "% for median and p95):\n";
$materialWinFailures = [];
foreach (MATERIAL_WIN_WORKLOADS as $name) {
    $materialWin = $allResults[$name]['material_win'] ?? null;
    if (!is_array($materialWin)) {
        printf("%-13s MISSING\n", $name);
        $materialWinFailures[] = $name;
        continue;
    }
    printf(
        "%-13s median %7.2f%%  p95 %7.2f%%  %s\n",
        $name,
        $materialWin['median_percent'],
        $materialWin['p95_percent'],
        $materialWin['passes'] ? 'PASS' : 'FAIL',
    );
    if ($materialWin['passes'] !== true) {
        $materialWinFailures[] = $name;
    }
}

$document = ['metadata' => $metadata, 'workloads' => $allResults];
$jsonPath = getenv('BENCH_JSON');
if (is_string($jsonPath) && $jsonPath !== '') {
    file_put_contents($jsonPath, json_encode($document, JSON_PRETTY_PRINT | JSON_THROW_ON_ERROR) . "\n");
}
if ($materialWinFailures !== []) {
    fwrite(STDERR, 'archived templates missed the material-win threshold for: '
        . implode(', ', $materialWinFailures) . "\n");
    exit(1);
}
