# Benchmark-framework validation

## Scope

This report records the project-owned integration of benchmark-framework `v1.0.0` for `bignum_prime_gen`. The downloaded distribution is stored under `libs/benchmark-framework/dist`; adapter callbacks include `prime-gen` vocabulary validation, deterministic state initialization, operation execution and observable checksum generation.

## Validated artifacts

| Artifact | Validation |
|---|---|
| `benchmarks/adapter/bignum_prime_gen_benchmark_adapter.c` | Compiled and exercised through adapter test and ST/MT runners |
| `benchmarks/profiles/bignum_prime_gen_standard.json` | Parsed by matrix tool; two CI-smoke profiles |
| `benchmarks/profiles/bignum_prime_gen_full.json` | JSON parse and vocabulary review; intended for controlled long runs |
| `benchmarks/bench_bignum_prime_gen.c` | ST runner completed with machine-readable output |
| `benchmarks/bench_bignum_prime_gen_mt.c` | MT runner completed with machine-readable output |

## ST and MT protocol

Every successful runner emits one `benchmark=...` record immediately before `Benchmark finished.`. The following explicit workload was used to keep the validation deterministic and fast:

```bash
bin/bench_bignum_prime_gen --input-kind random --operation-kind prime-gen \
  --measure-mode kernel-only --size-profile tiny --capacity-profile normal \
  --iterations 1 --warmup 0 --data-count 1 --seed 1

bin/bench_bignum_prime_gen_mt --input-kind random --operation-kind prime-gen \
  --measure-mode kernel-only --size-profile tiny --capacity-profile normal \
  --threads 2 --total-iterations 2 --warmup 0 --data-count 1 --seed 1
```

Both commands returned exit status 0 and the required completion marker.

## Legacy data modes

The framework's `all_zero`, `all_nonzero` and `mixed` data-mode commands were each checked in both ST and MT mode with explicit prime-gen workload parameters. The six combinations returned exit status 0, successful sample counts and valid completion markers. `data_mode=custom` is intentional: the prime-generation adapter uses its own random/mixed candidate semantics rather than treating zero-valued input as a meaningful prime-generation request.

| Data mode | ST | MT |
|---|---:|---:|
| `all_zero` | PASS | PASS |
| `all_nonzero` | PASS | PASS |
| `mixed` | PASS | PASS |

## JSON matrix

The standard matrix was run with one repetition, two measured ST iterations, four total MT iterations, one data record and a 120-second per-profile timeout:

```bash
make bench_matrix CONFIG=release USE_ASM=auto \
  BENCH_MATRIX_PROFILE=benchmarks/profiles/bignum_prime_gen_standard.json \
  BENCH_MATRIX_REPETITIONS=1 BENCH_MATRIX_ITERATIONS=2 \
  BENCH_MATRIX_MT_TOTAL_ITERATIONS=4 BENCH_MATRIX_WARMUP=0 \
  BENCH_MATRIX_DATA_COUNT=1 BENCH_MATRIX_TIMEOUT_SECONDS=120
```

The matrix produced four samples: two profiles times ST/MT. `benchmark_stats` aggregated all four groups and returned success.

The full profile intentionally includes medium/large workloads and is not part of the short CI smoke command. Those profiles require a controlled timeout and sufficient runtime because each operation performs cryptographic candidate generation and Miller--Rabin validation.

## Environment note

The Makefile's `bench_cl` target requires a kernel-matched `perf` executable. That binary is unavailable in the current sandbox, so the target cannot run here. This is an environment limitation, not a framework or adapter failure. The direct runners and no-PMU JSON matrix provide the reproducible validation evidence available in this environment.
