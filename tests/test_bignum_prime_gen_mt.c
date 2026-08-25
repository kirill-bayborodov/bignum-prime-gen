#include "bignum_prime_gen.h"

#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define NUM_THREADS 8

typedef struct prime_gen_thread_data {
    bignum_t value;
    size_t bits;
    bignum_prime_gen_status_t status;
} prime_gen_thread_data_t;

static size_t bit_length(const bignum_t *value)
{
    size_t bits = value->len * 64U;
    uint64_t top;
    if (value->len == 0U) return 0U;
    top = value->words[value->len - 1U];
    while ((top >> 63U) == 0U) { top <<= 1U; --bits; }
    return bits;
}

static void *worker_thread(void *argument)
{
    prime_gen_thread_data_t *data = argument;
    data->status = bignum_prime_gen(&data->value, data->bits, 5U, 4096U);
    return NULL;
}

int main(void)
{
    pthread_t threads[NUM_THREADS];
    prime_gen_thread_data_t data[NUM_THREADS];
    for (size_t index = 0U; index < NUM_THREADS; ++index) {
        memset(&data[index], 0, sizeof(data[index]));
        data[index].bits = 32U + index * 8U;
        assert(pthread_create(&threads[index], NULL, worker_thread, &data[index]) == 0);
    }
    for (size_t index = 0U; index < NUM_THREADS; ++index) {
        assert(pthread_join(threads[index], NULL) == 0);
        assert(data[index].status == BIGNUM_PRIME_GEN_SUCCESS);
        assert(bit_length(&data[index].value) == data[index].bits);
        assert((data[index].value.words[0] & UINT64_C(1)) != 0U);
    }
    puts("bignum_prime_gen MT tests: OK");
    return 0;
}
