# How-to: `bignum_prime_gen_full.json`

## Назначение

`bignum_prime_gen_full.json` — расширенная domain-specific matrix для анализа производительности генерации простых чисел заданной битовой длины. Она предназначена для подготовленного controlled run, а не для быстрого CI smoke. Manifest сохраняет meaningful workload axes: random/mixed input metadata, exact `prime-gen` operation, candidate bit length, measurement boundary and near-capacity state.

The C11 `bench_matrix` runner from pinned `benchmark-framework v1.0.0` accepts the JSON document and launches project-owned ST/MT bignum adapter binaries. The runner writes a raw samples document; the C11 `benchmark_stats` tool parses it through public `json-lib` and emits a metrics/regression summary.

## Coverage

| Family | Profiles | What it isolates |
|---|---:|---|
| Tiny/small candidates | 3 | Low-cost prime generation at 32 and 64 bits |
| Medium/large candidates | 4 | Prime generation cost at 256 and 1024 bits |
| Variable candidates | 2 | Reproducible variable bit-length workload behavior |
| Near-capacity | 2 | Safe upper-bound candidate generation within bignum capacity |

The document declares **11 profiles**. A run with `R` repetitions therefore produces `11 × 2 × R` samples: one ST and one MT process per profile/repetition.

## Controlled full run

Use fixed seed, thread count, data-count and iteration counts when a result will become a baseline. The following command shows the expected C11 runner contract; Makefile integration will expose the same parameters after its separate approval.

```bash
libs/benchmark-framework/dist/tools/bench_matrix \
  --manifest benchmarks/profiles/bignum_prime_gen_full.json \
  --output benchmarks/reports/bignum_prime_gen_full_matrix.json \
  --st-binary bin/bench_bignum_prime_gen \
  --mt-binary bin/bench_bignum_prime_gen_mt \
  --repetitions 7 \
  --iterations 200000000 \
  --mt-total-iterations 320000000 \
  --threads 2 \
  --warmup 10000 \
  --data-count 4096 \
  --seed 11400714819323198485 \
  --timeout-seconds 1800
```

Do not compare this result to data collected with different manifest contents, compiler configuration, CPU affinity, thread count or benchmark boundary. The JSON report records profile text, command/protocol outputs and individual timing samples so the conditions remain auditable.

## Review candidate metrics

Create a candidate summary first:

```bash
libs/benchmark-framework/dist/tools/benchmark_stats \
  --input benchmarks/reports/bignum_prime_gen_full_matrix.json \
  --output benchmarks/reports/bignum_prime_gen_full_summary.json
```

After review, preserve the raw matrix JSON as the baseline because it contains all repetitions and profile metadata. Compare a later candidate as follows:

```bash
libs/benchmark-framework/dist/tools/benchmark_stats \
  --input benchmarks/reports/candidate_full_matrix.json \
  --baseline benchmarks/reports/reviewed_full_matrix.json \
  --output benchmarks/reports/candidate_full_summary.json \
  --threshold-pct 5
```

A `regression:true` field means the candidate median exceeded both the configured threshold and robust MAD-based noise floor. A non-zero result with `missing_profiles` means the documents do not share complete profile/mode coverage and must not be treated as a valid comparison.

## Bignum transport vocabulary

`operation_kind` is exactly `prime-gen`. Generic example values such as `xor` or `rotate` are not legal for this adapter. The adapter validates this token before it initializes bignum state, therefore malformed profiles fail before their data become benchmark samples.

| `operation_kind` | Adapter behavior |
|---|---|
| `prime-gen` | Generates a candidate prime at the profile-selected bit length |

The adapter accepts `size_profile` values `tiny`, `small`, `medium`, `large` and `variable`; `capacity_profile` remains `normal` or `near-capacity`. The `input_kind` values `random` and `mixed` describe deterministic workload metadata and do not replace the cryptographic candidate generation performed by the operation.
