# Benchmarks

`archive-cache-vs-source.php` measures libmustache's protected archived-template format across the PHP and APCu cache boundary. The archived-template methods are compiled only into the dedicated benchmark package; they are not part of the normal extension API.

Run the complete benchmark with:

```console
nix run .#php83-archive-benchmark
```

The defaults are 101 timed samples after 10 warmups and 31 fresh-process samples. The comparison covers deterministic flat and nested-partial workloads near 1 KiB, 32 KiB, and 256 KiB. It reports source compilation and archive loading, PHP serialization, APCu cache hits and writes, payload size, and PHP peak memory.

Set `BENCH_JSON` to retain the complete result document. `BENCH_SAMPLES`, `BENCH_WARMUPS`, and `BENCH_COLD_SAMPLES` can shorten exploratory runs. For example:

```console
BENCH_SAMPLES=21 BENCH_WARMUPS=3 BENCH_COLD_SAMPLES=0 \
  nix run .#php83-archive-benchmark
```

The predeclared material-win threshold is at least 20% lower median and p95 APCu cache-hit latency for every medium and large flat and nested-partial workload. The command exits nonzero when any required workload misses that threshold. Writer cost, payload growth, and peak memory remain explicit tradeoffs rather than pass/fail criteria.
