#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
OUT=${1:-"$ROOT/coverage/c11"}
CC=${CC:-gcc}
CFLAGS="-std=c11 -Wall -Wextra -pedantic -O0 -g -fprofile-arcs -ftest-coverage"
INCLUDES="-I$ROOT/include -I$ROOT/libs/bignum-core/include -I$ROOT/libs/bignum-is-prime/include -I$ROOT/libs/bignum-random/include"
LIBS="$ROOT/libs/bignum-is-prime/build/bignum_is_prime.o $ROOT/libs/bignum-random/build/bignum_random.o -pthread"

rm -rf "$OUT"
mkdir -p "$OUT"
cd "$ROOT"

$CC $CFLAGS $INCLUDES -c src/bignum_prime_gen.c -o "$OUT/bignum_prime_gen.o"
for test in test_bignum_prime_gen test_bignum_prime_gen_extra test_bignum_prime_gen_mt test_bignum_prime_gen_runner; do
    $CC $CFLAGS $INCLUDES "tests/$test.c" "$OUT/bignum_prime_gen.o" $LIBS -no-pie -o "$OUT/$test"
    "$OUT/$test"
done

GCOV_PREFIX="$OUT" gcov -b -c -o "$OUT" src/bignum_prime_gen.c > "$OUT/gcov.txt"
cat "$OUT/gcov.txt"
printf '%s\n' 'production_source_entries=1'
