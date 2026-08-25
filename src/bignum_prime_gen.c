/**
 * @file bignum_prime_gen.c
 * @brief C11 reference implementation of exact-bit-length prime generation.
 */
#include "bignum_prime_gen.h"

#include <string.h>

#include <bignum_is_prime.h>
#include <bignum_random.h>

/** @brief Build the exclusive upper bound 2^bits for candidate sampling. */
static void bignum_prime_gen_make_bound(bignum_t *bound, size_t bits)
{
    const size_t word_index = (bits - 1U) / 64U;
    const size_t bit_index = (bits - 1U) % 64U;

    memset(bound, 0, sizeof(*bound));
    bound->words[word_index] = UINT64_C(1) << bit_index;
    bound->len = word_index + 1U;
}

/** @brief Set the requested highest bit and the oddness bit of a candidate. */
static void bignum_prime_gen_shape_candidate(bignum_t *candidate, size_t bits)
{
    const size_t word_index = (bits - 1U) / 64U;
    const size_t bit_index = (bits - 1U) % 64U;

    candidate->words[word_index] |= UINT64_C(1) << bit_index;
    candidate->words[0] |= UINT64_C(1);
    candidate->len = word_index + 1U;
}

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
