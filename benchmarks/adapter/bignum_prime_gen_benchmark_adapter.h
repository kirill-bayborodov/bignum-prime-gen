/**
 * @file bignum_prime_gen_benchmark_adapter.h
 * @brief benchmark-core adapter contract for exact-bit-length prime generation.
 * @details This header connects benchmark-framework transport records to the
 * project-owned bignum_prime_gen operation. The adapter owns no caller memory,
 * performs no allocation, and is safe for independent benchmark instances.
 * The framework owns the adapter object during a run; callback state storage is
 * caller-provided by benchmark-core and remains valid for the run lifecycle.
 */
#ifndef BIGNUM_PRIME_GEN_BENCHMARK_ADAPTER_H
#define BIGNUM_PRIME_GEN_BENCHMARK_ADAPTER_H

#include <benchmark_framework.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports adapter validation, setup, or operation status.
 * @details Successful initialization guarantees every callback field required by
 * benchmark-core is populated. Failure leaves caller-owned adapter storage
 * unchanged unless the caller discards the failed initialization object.
 */
typedef enum bignum_prime_gen_benchmark_status {
    BIGNUM_PRIME_GEN_BENCHMARK_SUCCESS = 0, /**< Setup or validation completed; outputs are valid. */
    BIGNUM_PRIME_GEN_BENCHMARK_NULL_ARGUMENT = 1, /**< A required pointer is NULL; no pointed object is dereferenced. */
    BIGNUM_PRIME_GEN_BENCHMARK_INVALID_PROFILE = 2, /**< A workload token is unsupported; no benchmark operation is started. */
    BIGNUM_PRIME_GEN_BENCHMARK_OPERATION_ERROR = 3 /**< Prime generation failed during a callback; the sample is not valid. */
} bignum_prime_gen_benchmark_status_t;

/**
 * @brief Initializes the benchmark-core adapter callbacks and state size.
 * @details The adapter is zeroed before callback fields are installed. The
 * caller retains ownership of `adapter`; no allocation occurs.
 * @param[out] adapter Caller-allocated benchmark-core adapter record; non-NULL.
 * @return `BIGNUM_PRIME_GEN_BENCHMARK_SUCCESS` when initialized, or
 * `BIGNUM_PRIME_GEN_BENCHMARK_NULL_ARGUMENT` when `adapter` is NULL.
 * @pre `adapter` designates writable storage for one benchmark_adapter_t.
 * @post A successful result provides the prime-gen callback table and state size.
 * @warning The object must remain alive and unmodified while benchmark-core runs.
 * @thread_safety Safe for independent adapter records; not safe for concurrent mutation of one record.
 */
bignum_prime_gen_benchmark_status_t bignum_prime_gen_benchmark_adapter_init(
    benchmark_adapter_t *adapter);

/**
 * @brief Validates all benchmark workload vocabulary accepted by this adapter.
 * @details Accepted operation_kind is exactly `prime-gen`; size and capacity
 * tokens determine candidate bit-length metadata but never bypass API bounds.
 * @param[in] workload Borrowed framework workload record; NULL is rejected.
 * @return Named adapter status; the workload is never modified.
 * @pre String fields remain valid for the duration of the call.
 * @post Success means every token is supported by the project adapter.
 * @thread_safety Read-only and reentrant for independent workload records.
 */
bignum_prime_gen_benchmark_status_t bignum_prime_gen_benchmark_validate_workload(
    const benchmark_workload_t *workload);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_PRIME_GEN_BENCHMARK_ADAPTER_H */
