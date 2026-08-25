/**
 * @file bignum_prime_gen_benchmark_adapter.h
 * @brief benchmark-core adapter for exact-bit-length prime generation.
 */
#ifndef BIGNUM_PRIME_GEN_BENCHMARK_ADAPTER_H
#define BIGNUM_PRIME_GEN_BENCHMARK_ADAPTER_H

#include <benchmark_core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum bignum_prime_gen_benchmark_status {
    BIGNUM_PRIME_GEN_BENCHMARK_SUCCESS = 0,
    BIGNUM_PRIME_GEN_BENCHMARK_NULL_ARGUMENT = 1,
    BIGNUM_PRIME_GEN_BENCHMARK_INVALID_PROFILE = 2,
    BIGNUM_PRIME_GEN_BENCHMARK_OPERATION_ERROR = 3
} bignum_prime_gen_benchmark_status_t;

bignum_prime_gen_benchmark_status_t bignum_prime_gen_benchmark_adapter_init(
    benchmark_adapter_t *adapter);

bignum_prime_gen_benchmark_status_t bignum_prime_gen_benchmark_validate_workload(
    const benchmark_workload_t *workload);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_PRIME_GEN_BENCHMARK_ADAPTER_H */
