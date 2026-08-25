#include "bignum_prime_gen.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    bignum_t value;
    memset(&value, 0, sizeof(value));
    assert(bignum_prime_gen(&value, 32U, 5U, 4096U) == BIGNUM_PRIME_GEN_SUCCESS);
    assert(value.len == 1U);
    assert((value.words[0] & UINT64_C(1)) != 0U);
    puts("bignum_prime_gen runner: OK");
    return 0;
}
