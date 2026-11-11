#ifndef KS_TEST_H
#define KS_TEST_H

/*
 * Minimal test helpers. A test file defines main(), calls CHECK... macros, and ends with
 * "return ks_test_result();". A failed check prints file:line and makes the test exit non-zero.
 */

#include <stdio.h>
#include <string.h>

static int ks_test_failures;

#define CHECK(cond)                                                                                \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            printf("%s:%d: CHECK failed: %s\n", __FILE__, __LINE__, #cond);                        \
            ks_test_failures++;                                                                    \
        }                                                                                          \
    } while (0)

#define CHECK_EQ_INT(actual, expected)                                                             \
    do {                                                                                           \
        long long ks_a = (long long)(actual);                                                      \
        long long ks_e = (long long)(expected);                                                    \
        if (ks_a != ks_e) {                                                                        \
            printf("%s:%d: %s is %lld, expected %lld\n", __FILE__, __LINE__, #actual, ks_a, ks_e); \
            ks_test_failures++;                                                                    \
        }                                                                                          \
    } while (0)

#define CHECK_EQ_STR(actual, expected)                                                             \
    do {                                                                                           \
        const char *ks_a = (actual);                                                               \
        const char *ks_e = (expected);                                                             \
        if (strcmp(ks_a, ks_e) != 0) {                                                             \
            printf("%s:%d: %s is \"%s\", expected \"%s\"\n", __FILE__, __LINE__, #actual, ks_a,    \
                   ks_e);                                                                          \
            ks_test_failures++;                                                                    \
        }                                                                                          \
    } while (0)

static int ks_test_result(void) {
    if (ks_test_failures) {
        printf("FAILED: %d check(s)\n", ks_test_failures);
        return 1;
    }
    printf("ok\n");
    return 0;
}

#endif
