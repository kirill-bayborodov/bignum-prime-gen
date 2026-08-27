# How-to: `bignum_prime_gen_standard.json`

## Назначение

`bignum_prime_gen_standard.json` — компактная versioned matrix для функциональной проверки и регрессионного baseline операции `bignum_prime_gen`. Manifest использует schema version `1`, которую читает pinned `benchmark-framework v1.0.0`.

> Manifest описывает exact-bit-length probable-prime generation через нейтральные transport fields benchmark framework.

| JSON field | Допустимые значения | Bignum interpretation |
|---|---|---|
| `input_kind` | `random`, `mixed` | Источник candidate workload dataset |
| `operation_kind` | `prime-gen` | Единственная операция этого adapter |
| `measure_mode` | `end-to-end`, `kernel-only` | Полный lifecycle либо timed generation interval |
| `size_profile` | `tiny`, `small`, `medium`, `large`, `variable` | Запрошенная разрядность candidate |
| `capacity_profile` | `normal`, `near-capacity` | Рабочая область и boundary metadata |

## Пошаговый smoke run

```bash
libs/benchmark-framework/dist/tools/bench_matrix \
  --manifest benchmarks/profiles/bignum_prime_gen_standard.json \
  --output benchmarks/reports/bignum_prime_gen_standard_matrix.json \
  --st-binary bin/bench_bignum_prime_gen \
  --mt-binary bin/bench_bignum_prime_gen_mt \
  --repetitions 1 \
  --iterations 2 \
  --mt-total-iterations 4 \
  --threads 2 \
  --warmup 0 \
  --data-count 1 \
  --seed 11400714819323198485 \
  --timeout-seconds 120
```

Every accepted sample has exactly one machine-readable `benchmark=...` line before its `Benchmark finished.` marker. For production performance studies, increase repetitions and iterations only after validating that selected bit sizes fit the available runtime budget.

## Aggregation and comparison

```bash
libs/benchmark-framework/dist/tools/benchmark_stats \
  --input benchmarks/reports/bignum_prime_gen_standard_matrix.json \
  --output benchmarks/reports/bignum_prime_gen_standard_summary.json
```

A candidate comparison must use the same manifest, seed, repetitions, iteration counts, thread count and host affinity as the reviewed baseline. The statistics tool rejects missing or extra profile IDs instead of treating a partial comparison as valid.

## Boundary policy

`near-capacity` is metadata for a valid large candidate workload. It does not intentionally exercise invalid `bits` or output-capacity errors; those cases belong in deterministic API tests. Generation is bounded by `max_attempts`, and a failed attempt sequence must preserve the caller-owned output record.
