#include "bignum_prime_gen.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static size_t bit_length(const bignum_t *value)
{
    uint64_t top;
    size_t bits;
    if (value->len == 0U) return 0U;
    bits = value->len * 64U;
    top = value->words[value->len - 1U];
    while ((top >> 63U) == 0U) { top <<= 1U; --bits; }
    return bits;
}

static void test_invalid_contracts(void)
{
    bignum_t value;
    bignum_t original;
    memset(&value, 0x5A, sizeof(value));
    original = value;
    assert(bignum_prime_gen(NULL, 8U, 8U, 32U) == BIGNUM_PRIME_GEN_ERROR_NULL_ARG);
    assert(bignum_prime_gen(&value, 0U, 8U, 32U) == BIGNUM_PRIME_GEN_ERROR_BITS);
    assert(bignum_prime_gen(&value, BIGNUM_CAPACITY * 64U + 1U, 8U, 32U) ==
        BIGNUM_PRIME_GEN_ERROR_BITS);
    assert(bignum_prime_gen(&value, 8U, 0U, 32U) == BIGNUM_PRIME_GEN_ERROR_ROUNDS);
    assert(bignum_prime_gen(&value, 8U, 8U, 0U) == BIGNUM_PRIME_GEN_ERROR_ATTEMPTS);
    assert(memcmp(&value, &original, sizeof(value)) == 0);
}

static void test_exact_lengths_and_normalization(void)
{
    const size_t requested[] = {2U, 3U, 7U, 31U, 63U, 65U, 127U, 256U};
    for (size_t index = 0U; index < sizeof(requested) / sizeof(requested[0]); ++index) {
        bignum_t value;
        memset(&value, 0, sizeof(value));
        assert(bignum_prime_gen(&value, requested[index], 5U, 4096U) ==
            BIGNUM_PRIME_GEN_SUCCESS);
        assert(bit_length(&value) == requested[index]);
        assert(value.len == (requested[index] + 63U) / 64U);
        assert((value.words[0] & UINT64_C(1)) != 0U);
        assert(value.words[value.len - 1U] != 0U);
        for (size_t word = value.len; word < BIGNUM_CAPACITY; ++word)
            assert(value.words[word] == 0U);
    }
}

static void test_maximum_capacity_generation(void)
{
    bignum_t value;
    memset(&value, 0, sizeof(value));
    assert(bignum_prime_gen(&value, 128U, 4U, 4096U) ==
        BIGNUM_PRIME_GEN_SUCCESS);
    assert(value.len == 2U);
    assert(bit_length(&value) == 128U);
    assert((value.words[0] & UINT64_C(1)) != 0U);
}

static void test_attempt_limit_preserves_output_on_failure(void)
{
    bignum_t value;
    bignum_t original;
    memset(&value, 0xA5, sizeof(value));
    original = value;
    {
        const bignum_prime_gen_status_t status =
            bignum_prime_gen(&value, 64U, 1U, 1U);
        assert(status == BIGNUM_PRIME_GEN_ERROR_PRIMALITY ||
            status == BIGNUM_PRIME_GEN_SUCCESS);
        if (status == BIGNUM_PRIME_GEN_ERROR_PRIMALITY)
            assert(memcmp(&value, &original, sizeof(value)) == 0);
    }
}

int main(void)
{
    test_invalid_contracts();
    test_exact_lengths_and_normalization();
    test_maximum_capacity_generation();
    test_attempt_limit_preserves_output_on_failure();
    puts("bignum_prime_gen extra tests: OK");
    return 0;
}
