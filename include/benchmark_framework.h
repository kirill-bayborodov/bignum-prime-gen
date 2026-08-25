/**
 * @file benchmark_framework.h
 * @brief Public include bridge for the benchmark-framework distribution.
 * @details
 * The current dist artifact publishes this header at
 * `libs/benchmark-framework/dist/benchmark_framework.h`. A legacy local cache
 * may still expose the equivalent API as `dist/include/benchmark_core.h`.
 * This bridge contains no framework implementation and only normalizes the
 * distribution's packaging location for project-owned benchmark sources.
 */
#ifndef BIGNUM_PRIME_GEN_BENCHMARK_FRAMEWORK_BRIDGE_H
#define BIGNUM_PRIME_GEN_BENCHMARK_FRAMEWORK_BRIDGE_H

#if defined(__has_include)
#  if __has_include("../libs/benchmark-framework/dist/benchmark_framework.h")
#    include "../libs/benchmark-framework/dist/benchmark_framework.h"
#  else
#    include "../libs/benchmark-framework/dist/include/benchmark_core.h"
#  endif
#else
#  include "../libs/benchmark-framework/dist/benchmark_framework.h"
#endif

#endif /* BIGNUM_PRIME_GEN_BENCHMARK_FRAMEWORK_BRIDGE_H */
