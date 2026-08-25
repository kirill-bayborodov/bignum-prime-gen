#include "bignum_prime_gen.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static size_t bit_length(const bignum_t *value)
{
    uint64_t top;
    size_t bits = value->len * 64U;
    if (value->len == 0U) return 0U;
    top = value->words[value->len - 1U];
    while ((top >> 63U) == 0U) { top <<= 1U; --bits; }
    return bits;
}

static void test_small_generation(void)
{
    bignum_t value;
    memset(&value, 0, sizeof(value));
    assert(bignum_prime_gen(&value, 8U, 8U, 4096U) == BIGNUM_PRIME_GEN_SUCCESS);
    assert(bit_length(&value) == 8U);
    assert((value.words[0] & 1U) != 0U);
}

static void test_multiword_generation(void)
{
    bignum_t value;
    memset(&value, 0, sizeof(value));
    assert(bignum_prime_gen(&value, 65U, 8U, 4096U) == BIGNUM_PRIME_GEN_SUCCESS);
    assert(bit_length(&value) == 65U);
    assert((value.words[0] & 1U) != 0U);
}

static void test_invalid_arguments_preserve_output(void)
{
    bignum_t value;
    bignum_t original;
    memset(&value, 0xA5, sizeof(value));
    original = value;
    assert(bignum_prime_gen(NULL, 8U, 8U, 32U) == BIGNUM_PRIME_GEN_ERROR_NULL_ARG);
    assert(bignum_prime_gen(&value, 1U, 8U, 32U) == BIGNUM_PRIME_GEN_ERROR_BITS);
    assert(value.words[0] == original.words[0]);
    assert(memcmp(&value, &original, sizeof(value)) == 0);
    assert(bignum_prime_gen(&value, 8U, 0U, 32U) == BIGNUM_PRIME_GEN_ERROR_ROUNDS);
    assert(bignum_prime_gen(&value, 8U, 8U, 0U) == BIGNUM_PRIME_GEN_ERROR_ATTEMPTS);
    assert(memcmp(&value, &original, sizeof(value)) == 0);
}

static void test_boundary_generation(void)
{
    bignum_t value;
    memset(&value, 0, sizeof(value));
    assert(bignum_prime_gen(&value, 65U, 4U, 4096U) ==
        BIGNUM_PRIME_GEN_SUCCESS);
    assert(bit_length(&value) == 65U);
    assert((value.words[0] & 1U) != 0U);
}

int main(void)
{
    test_small_generation();
    test_multiword_generation();
    test_invalid_arguments_preserve_output();
    test_boundary_generation();
    puts("bignum_prime_gen deterministic tests: OK");
    return 0;
}
