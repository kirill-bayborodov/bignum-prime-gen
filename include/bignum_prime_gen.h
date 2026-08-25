/**
 * @file bignum_prime_gen.h
 * @brief Generate a probable prime with an exact requested bit length.
 * @details The module samples candidates through bignum_random and validates
 * them with bignum_is_prime. The caller owns all input/output storage; no heap
 * allocation or mutable global state is used. The C11 implementation is the
 * correctness reference and the assembly implementation preserves this API.
 */
#ifndef BIGNUM_PRIME_GEN_H
#define BIGNUM_PRIME_GEN_H

#include <bignum.h>
#include <stddef.h>

#ifndef BIGNUM_CAPACITY
#error "bignum.h must define BIGNUM_CAPACITY"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the outcome of exact-bit-length probable-prime generation.
 * @details A failure leaves the complete caller-owned output record unchanged.
 */
typedef enum bignum_prime_gen_status {
    BIGNUM_PRIME_GEN_SUCCESS = 0,
    BIGNUM_PRIME_GEN_ERROR_NULL_ARG = -1,
    BIGNUM_PRIME_GEN_ERROR_BITS = -2,
    BIGNUM_PRIME_GEN_ERROR_ROUNDS = -3,
    BIGNUM_PRIME_GEN_ERROR_ATTEMPTS = -4,
    BIGNUM_PRIME_GEN_ERROR_RANDOM = -5,
    BIGNUM_PRIME_GEN_ERROR_PRIMALITY = -6
} bignum_prime_gen_status_t;

/**
 * @brief Generates a probable prime with exactly `bits` significant bits.
 * @param[out] out Destination record written only on success.
 * @param[in] bits Requested bit length in the inclusive range
 *                 [2, BIGNUM_CAPACITY * 64].
 * @param[in] rounds Positive Miller--Rabin round count passed to
 *                    bignum_is_prime.
 * @param[in] max_attempts Positive upper bound on candidate attempts.
 * @return A named bignum_prime_gen_status_t value.
 * @details Each candidate has its highest requested bit and least significant
 * bit set, so accepted output has the requested bit length and is odd. The
 * random source is rejection-based and candidates are tested independently.
 * If no prime is found before `max_attempts`, the output is unchanged and
 * BIGNUM_PRIME_GEN_ERROR_PRIMALITY is returned.
 * @pre `out` is a live caller-owned bignum_t; all scalar parameters are valid.
 * @post On success, `out` is normalized, odd, and has exactly `bits` bits.
 * @thread_safety Independent calls are reentrant and safe concurrently.
 */
bignum_prime_gen_status_t bignum_prime_gen(
    bignum_t *out,
    size_t bits,
    size_t rounds,
    size_t max_attempts);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_PRIME_GEN_H */
