/**
 * @file bignum_prime_gen.c
 * @brief C11 reference implementation of exact-bit-length prime generation.
 * @details The reference constructs an exclusive power-of-two bound, samples
 * candidates with bignum_random, forces the requested high bit and oddness, and
 * validates each candidate with bignum_is_prime. Temporary records are stack
 * allocated; the caller-owned output is published only after success.
 */
#include "bignum_prime_gen.h"

#include <string.h>

#include <bignum_is_prime.h>
#include <bignum_random.h>

/**
 * @brief Builds the normalized exclusive bound 2^bits.
 * @details The set bit is placed in the highest requested word; zeroing first
 * protects the bignum normalization invariant required by bignum_random.
 * @param[out] bound Caller-provided temporary record receiving the bound.
 * @param[in] bits Requested bit length, already validated to fit capacity.
 * @return None; `bound` is fully initialized on return.
 */
static void bignum_prime_gen_make_bound(bignum_t *bound, size_t bits)
{
    const size_t word_index = (bits - 1U) / 64U;
    const size_t bit_index = (bits - 1U) % 64U;

    memset(bound, 0, sizeof(*bound));
    bound->words[word_index] = UINT64_C(1) << bit_index;
    bound->len = word_index + 1U;
}

/**
 * @brief Shapes one sampled candidate for exact length and oddness.
 * @details Setting the top bit prevents a shorter result; setting word zero's
 * low bit excludes even candidates before the primality test.
 * @param[in,out] candidate Temporary sampled record; caller retains ownership.
 * @param[in] bits Requested bit length within bignum_t capacity.
 * @return None; candidate length and significant bits are normalized.
 */
static void bignum_prime_gen_shape_candidate(bignum_t *candidate, size_t bits)
{
    const size_t word_index = (bits - 1U) / 64U;
    const size_t bit_index = (bits - 1U) % 64U;

    candidate->words[word_index] |= UINT64_C(1) << bit_index;
    candidate->words[0] |= UINT64_C(1);
    candidate->len = word_index + 1U;
}

/**
 * @brief Generates a bounded exact-bit-length probable-prime candidate.
 * @details Each attempt samples below 2^bits, shapes the candidate, invokes
 * Miller--Rabin, and publishes only a passing candidate. Random-source and
 * primality errors are mapped to named generator statuses; retry exhaustion is
 * reported as BIGNUM_PRIME_GEN_ERROR_PRIMALITY. No heap allocation is used.
 * @return Named bignum_prime_gen_status_t; only SUCCESS modifies `*out`.
 * @pre `out` points to a live writable bignum_t and scalar arguments are valid.
 * @post Success gives an odd normalized probable prime with exactly `bits` bits;
 * failure preserves the complete output record.
 * @warning Probabilistic primality and random retry count are data-dependent;
 * callers must select rounds and attempt policy appropriate to their threat model.
 * @thread_safety Independent output records may be processed concurrently.
 * @complexity O(max_attempts * primality_cost(bits)) time and O(1) bignum records.
 */
bignum_prime_gen_status_t bignum_prime_gen(
    bignum_t *out,
    size_t bits,
    size_t rounds,
    size_t max_attempts)
{
    bignum_t bound;
    bignum_t candidate;

    if (out == NULL) {
        return BIGNUM_PRIME_GEN_ERROR_NULL_ARG;
    }
    if (bits < 2U || bits > BIGNUM_CAPACITY * 64U) {
        return BIGNUM_PRIME_GEN_ERROR_BITS;
    }
    if (rounds == 0U) {
        return BIGNUM_PRIME_GEN_ERROR_ROUNDS;
    }
    if (max_attempts == 0U) {
        return BIGNUM_PRIME_GEN_ERROR_ATTEMPTS;
    }

    bignum_prime_gen_make_bound(&bound, bits);
    for (size_t attempt = 0U; attempt < max_attempts; ++attempt) {
        bignum_is_prime_status_t prime_status;
        bignum_random_status_t random_status;
        int is_prime = 0;

        /* Do not expose a random candidate: publication is intentionally delayed
         * until primality succeeds, preserving the caller's transactional contract. */
        random_status = bignum_random(&candidate, &bound);
        if (random_status != BIGNUM_RANDOM_SUCCESS) {
            return BIGNUM_PRIME_GEN_ERROR_RANDOM;
        }
        bignum_prime_gen_shape_candidate(&candidate, bits);
        prime_status = bignum_is_prime(&candidate, rounds, &is_prime);
        if (prime_status != BIGNUM_IS_PRIME_SUCCESS) {
            return BIGNUM_PRIME_GEN_ERROR_PRIMALITY;
        }
        if (is_prime != 0) {
            *out = candidate;
            return BIGNUM_PRIME_GEN_SUCCESS;
        }
    }
    return BIGNUM_PRIME_GEN_ERROR_PRIMALITY;
}
