# bignum-prime-gen Quality Review and Benchmark Report

## Scope

This report covers the C11 reference, x86-64 YASM implementation, public header, tests, benchmark adapter, JSON profiles and distribution layout at the current review revision.

## Quality Gate checklist

| Artifact | Documentation | Contract | Tests | Static result |
|---|---|---|---|---|
| `include/bignum_prime_gen.h` | Complete API/status/parameter/thread-safety documentation | Exact bits, rounds, attempts and transactional output documented | Public deterministic and invalid-argument vectors | PASS |
| `src/bignum_prime_gen.c` | Algorithm and failure semantics documented in header/source | C11 reference uses bounded random candidates and Miller--Rabin | C11 suite and coverage driver | PASS |
| `src/bignum_prime_gen.asm` | ABI, frame, registers and pipeline documented | Same public four-argument contract | ASM suite and runner | PASS |
| `tests/test_bignum_prime_gen.c` | Test intent documented | Valid, invalid and boundary cases | PASS | PASS |
| `tests/test_bignum_prime_gen_extra.c` | Extended cases documented | Retry and preservation cases | PASS | PASS |
| `tests/test_bignum_prime_gen_mt.c` | Reentrancy intent documented | Independent output records | PASS | PASS |
| `benchmarks/adapter/*` | Framework callback and status mapping documented | `prime-gen` vocabulary only | Adapter tests | PASS |
| `benchmarks/profiles/*.json` | Companion how-to documents present | JSON parses and uses schema version 1 | Python JSON validation | PASS |
| `README.md` | Build, API, test, coverage, benchmark and distribution sections | Matches project-specific API | Manual stale-reference scan | PASS |

## Validation evidence

The release C11 and ASM suites each completed all five binaries with `0 / 5 failed`. The single-source C11 coverage script instruments `src/bignum_prime_gen.c` once and reports:

```text
Lines executed: 97.30% of 37
Branches executed: 100.00% of 18
Calls executed: 100.00% of 4
production_source_entries=1
```

The benchmark-framework matrix smoke run produced four valid samples: two ST and two MT samples across tiny and small profiles. All samples contained a valid machine-readable benchmark record and completion marker.

## Non-PMU timing comparison

The Makefile `bench_cl` target was attempted. It is unavailable in this sandbox because the required kernel-matched `perf` binary is not installed. No PMU claim is made. As an equivalent software-time fallback, identical benchmark binaries were run with the same seed, profile, iteration count and data count; wall time was measured externally with Python `time.perf_counter()`.

| Implementation | Profile | Iterations | Reported ns/call | Process wall seconds |
|---|---|---:|---:|---:|
| C11 | tiny | 4 | 153,315.5 | 0.002129 |
| ASM | tiny | 4 | 350,439.25 | 0.002863 |
| C11 | small | 2 | 643,855.5 | 0.002616 |
| ASM | small | 2 | 429,132.5 | 0.002074 |

The tiny workload is dominated by process/startup and random-source noise. The small workload shows ASM faster in this short run. These figures are smoke-level evidence, not a statistically stable optimization claim; a real `bench_cl` study requires a compatible `perf` binary and substantially larger repetitions.

## Known environment exception

`make lint` scans nested dependency sources and reports an inherited missing include in `libs/bignum-random/include/benchmark_framework.h`. That dependency file is outside this repository's requested implementation scope. The project-owned adapter includes the downloaded public `benchmark_core.h` correctly.

## Review conclusion

Project-owned source references contain no stale template or shift vocabulary. C11 and ASM functional validation pass, production-source C11 line coverage exceeds 90%, JSON manifests parse, and benchmark-framework smoke integration passes. Before a production release, install the kernel-matched `perf` tool and rerun the long-form `bench_cl` matrix with at least seven repetitions.
