/**
 * @file bignum_prime_gen_benchmark_adapter.c
 * @brief benchmark-core adapter implementation for bignum_prime_gen.
 */
#include "bignum_prime_gen_benchmark_adapter.h"

#include <string.h>

#include <bignum.h>
#include "bignum_prime_gen.h"

#define PRIME_GEN_BENCHMARK_FNV_OFFSET UINT64_C(1469598103934665603)
#define PRIME_GEN_BENCHMARK_FNV_PRIME UINT64_C(1099511628211)

typedef struct bignum_prime_gen_benchmark_state {
    bignum_t value;
    size_t bits;
    size_t rounds;
    size_t max_attempts;
} bignum_prime_gen_benchmark_state_t;

static uint64_t prime_gen_benchmark_mix(uint64_t value)
{
    value += UINT64_C(0x9E3779B97F4A7C15);
    value = (value ^ (value >> 30U)) * UINT64_C(0xBF58476D1CE4E5B9);
    value = (value ^ (value >> 27U)) * UINT64_C(0x94D049BB133111EB);
    return value ^ (value >> 31U);
}

static int prime_gen_benchmark_allowed(const char *value,
    const char *const *allowed)
{
    if (value == NULL || allowed == NULL) return 0;
    for (size_t index = 0U; allowed[index] != NULL; ++index) {
        if (strcmp(value, allowed[index]) == 0) return 1;
    }
    return 0;
}

bignum_prime_gen_benchmark_status_t bignum_prime_gen_benchmark_validate_workload(
    const benchmark_workload_t *workload)
{
    static const char *const input_kinds[] = {"random", "mixed", NULL};
    static const char *const operation_kinds[] = {"prime-gen", NULL};
    static const char *const measure_modes[] = {"kernel-only", "end-to-end", NULL};
    static const char *const size_profiles[] = {"tiny", "small", "medium", "large", "variable", NULL};
    static const char *const capacity_profiles[] = {"normal", "near-capacity", NULL};

    if (workload == NULL) return BIGNUM_PRIME_GEN_BENCHMARK_NULL_ARGUMENT;
    if (!prime_gen_benchmark_allowed(workload->input_kind, input_kinds) ||
        !prime_gen_benchmark_allowed(workload->operation_kind, operation_kinds) ||
        !prime_gen_benchmark_allowed(workload->measure_mode, measure_modes) ||
        !prime_gen_benchmark_allowed(workload->size_profile, size_profiles) ||
        !prime_gen_benchmark_allowed(workload->capacity_profile, capacity_profiles)) {
        return BIGNUM_PRIME_GEN_BENCHMARK_INVALID_PROFILE;
    }
    return BIGNUM_PRIME_GEN_BENCHMARK_SUCCESS;
}

static size_t prime_gen_benchmark_bits(const benchmark_workload_t *workload,
    uint64_t sequence_index)
{
    if (strcmp(workload->size_profile, "tiny") == 0) return 32U;
    if (strcmp(workload->size_profile, "small") == 0) return 64U;
    if (strcmp(workload->size_profile, "medium") == 0) return 256U;
    if (strcmp(workload->size_profile, "large") == 0) return 1024U;
    return 64U + (size_t)((workload->seed + sequence_index) % 1985U);
}

static benchmark_adapter_status_t prime_gen_benchmark_initialize(
    void *record,
    uint64_t sequence_index,
    const benchmark_workload_t *workload,
    void *context)
{
    bignum_prime_gen_benchmark_state_t *state = record;
    (void)context;
    if (state == NULL || workload == NULL) return BENCHMARK_ADAPTER_STATUS_INPUT_ERROR;
    if (bignum_prime_gen_benchmark_validate_workload(workload) !=
        BIGNUM_PRIME_GEN_BENCHMARK_SUCCESS) return BENCHMARK_ADAPTER_STATUS_INPUT_ERROR;
    memset(state, 0, sizeof(*state));
    state->bits = prime_gen_benchmark_bits(workload, sequence_index);
    if (state->bits < 2U) state->bits = 2U;
    if (state->bits > BIGNUM_CAPACITY * 64U) state->bits = BIGNUM_CAPACITY * 64U;
    state->rounds = 3U;
    state->max_attempts = 128U;
    state->value.words[0] = prime_gen_benchmark_mix(workload->seed + sequence_index);
    state->value.len = 1U;
    return BENCHMARK_ADAPTER_STATUS_SUCCESS;
}

static benchmark_adapter_status_t prime_gen_benchmark_operation(
    void *record,
    uint64_t iteration,
    const benchmark_workload_t *workload,
    void *context)
{
    bignum_prime_gen_benchmark_state_t *state = record;
    (void)iteration;
    (void)workload;
    (void)context;
    if (state == NULL) return BENCHMARK_ADAPTER_STATUS_INPUT_ERROR;
    return bignum_prime_gen(&state->value, state->bits, state->rounds,
        state->max_attempts) == BIGNUM_PRIME_GEN_SUCCESS
        ? BENCHMARK_ADAPTER_STATUS_SUCCESS
        : BENCHMARK_ADAPTER_STATUS_OPERATION_ERROR;
}

static uint64_t prime_gen_benchmark_checksum(const void *record,
    uint64_t iteration, void *context)
{
    const bignum_prime_gen_benchmark_state_t *state = record;
    uint64_t checksum = PRIME_GEN_BENCHMARK_FNV_OFFSET ^ iteration;
    (void)context;
    if (state == NULL) return 0U;
    checksum ^= state->bits;
    for (size_t index = 0U; index < state->value.len; ++index) {
        checksum ^= state->value.words[index];
        checksum *= PRIME_GEN_BENCHMARK_FNV_PRIME;
    }
    return checksum;
}

bignum_prime_gen_benchmark_status_t bignum_prime_gen_benchmark_adapter_init(
    benchmark_adapter_t *adapter)
{
    if (adapter == NULL) return BIGNUM_PRIME_GEN_BENCHMARK_NULL_ARGUMENT;
    memset(adapter, 0, sizeof(*adapter));
    adapter->benchmark_name = "bignum_prime_gen";
    adapter->state_size = sizeof(bignum_prime_gen_benchmark_state_t);
    adapter->success_code = BENCHMARK_ADAPTER_STATUS_SUCCESS;
    adapter->initialize = prime_gen_benchmark_initialize;
    adapter->operation = prime_gen_benchmark_operation;
    adapter->checksum = prime_gen_benchmark_checksum;
    return BIGNUM_PRIME_GEN_BENCHMARK_SUCCESS;
}
