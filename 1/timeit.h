#ifndef TIMEIT_H
#define TIMEIT_H
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <sys/time.h>

#ifndef TIMEIT
#define TIMEIT(/* const char * */ NAME, CODE_TO_TEST) \
    do { \
        struct timespec timeit_start, timeit_end; \
        clock_gettime(CLOCK_MONOTONIC_RAW, &timeit_start); \
        { CODE_TO_TEST; } \
        clock_gettime(CLOCK_MONOTONIC_RAW, &timeit_end); \
        uint64_t timeit_delta_us = (timeit_end.tv_sec - timeit_start.tv_sec) * 1000000 + (timeit_end.tv_nsec - timeit_start.tv_nsec) / 1000; \
        printf("TIMEIT: %s took %" PRIu64 " usec\n", (NAME), timeit_delta_us); \
    } while (0)
#endif

#ifndef TIMEITN
#define TIMEITN(/* const char * */ NAME, /* unsigned */ N, CODE_TO_TEST) \
    do { \
        struct timespec timeit_start, timeit_end; \
        clock_gettime(CLOCK_MONOTONIC_RAW, &timeit_start); \
        for (unsigned timeit_i = 0; timeit_i < (N); ++timeit_i) { \
            { CODE_TO_TEST; } \
        } \
        clock_gettime(CLOCK_MONOTONIC_RAW, &timeit_end); \
        uint64_t timeit_delta_us = (timeit_end.tv_sec - timeit_start.tv_sec) * 1000000 + (timeit_end.tv_nsec - timeit_start.tv_nsec) / 1000; \
        printf("TIMEITN: %s took %" PRIu64 " usec for %u times\n", (NAME), timeit_delta_us, (N)); \
    } while (0)
#endif

#ifndef TIMEITN_FULL
#define TIMEITN_FULL(/* const char * */ NAME, /* unsigned */ N, PRECOND_CODE, CODE_TO_TEST, POSTCOND_CODE) \
    do { \
        uint64_t timeit_delta_us = 0; \
        for (unsigned timeit_i = 0; timeit_i < (N); ++timeit_i) { \
            struct timespec timeit_start, timeit_end; \
            { PRECOND_CODE; } \
            clock_gettime(CLOCK_MONOTONIC_RAW, &timeit_start); \
            { CODE_TO_TEST; }\
            clock_gettime(CLOCK_MONOTONIC_RAW, &timeit_end); \
            timeit_delta_us += (timeit_end.tv_sec - timeit_start.tv_sec) * 1000000 + (timeit_end.tv_nsec - timeit_start.tv_nsec) / 1000; \
            { POSTCOND_CODE; } \
        } \
        printf("TIMEITN_FULL: %s took %" PRIu64 " usec for %u times\n", (NAME), timeit_delta_us, (N)); \
    } while (0)
#endif
#endif

