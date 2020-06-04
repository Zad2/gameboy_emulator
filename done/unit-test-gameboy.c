/**
 * @file unit-test-gameboy.c
 * @brief Unit test code for gameboy and related functions
 *
 * @author L. James & K. Durant, EPFL
 * @date 2020 (corona-time)
 */

// for thread-safe randomization
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <check.h>
#include <inttypes.h>
#include <assert.h>

#include "tests.h"
#include "bus.h"
#include "memory.h"
#include "component.h"
#include "error.h"
#include "gameboy.h"
#include "cpu-storage.h"

#define INIT \
    gameboy_t g;    \
    memset(&g, 0, sizeof(gameboy_t))
//zero_init_var(g)

START_TEST(gameboy_create_err)
{
// ------------------------------------------------------------
#ifdef WITH_PRINT
    printf("=== %s:\n", __func__);
#endif
    INIT;
    gameboy_create(&g, "./tests/data/blargg_roms/01-special.gb");

    gameboy_free(&g);

#ifdef WITH_PRINT
    printf("=== END of %s\n", __func__);
#endif

}
END_TEST

Suite* bus_test_suite()
{

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
    srand(time(NULL) ^ getpid() ^ pthread_self());
#pragma GCC diagnostic pop

    Suite* s = suite_create("gameboy.c Tests");

    Add_Case(s, tc2, "gameboy tests");

    tcase_add_test(tc2, gameboy_create_err);

    return s;
}

TEST_SUITE(bus_test_suite)
