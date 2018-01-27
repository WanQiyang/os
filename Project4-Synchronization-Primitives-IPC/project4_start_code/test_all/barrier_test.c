#include "common.h"
#include "scheduler.h"
#include "sync.h"
#include "util.h"
#include "printf.h"

/* Threads to test barrier */

enum {
    NUM_THREADS = 3,
    NUM_ITERATIONS = 1000,
    MAX_SLEEP = 20,
    LINE = 20,                  /* Line where result should be printed */
    COL = 48                    /* Column where result should be printed */
};

static bool_t barrier_initialized = FALSE;
static barrier_t barrier;
static volatile int value[NUM_THREADS];

static void barrier_thread(int id)
{
    int i, j;

    if (0 == id) {
        barrier_init(&barrier, NUM_THREADS);
        barrier_initialized = TRUE;
    } else {
        while (!barrier_initialized) {
            do_yield();
        }
    }

    for (i = 0; i < NUM_ITERATIONS; ++i) {
        ++value[id];
        barrier_wait(&barrier);
        do_sleep(rand() % MAX_SLEEP);
        for (j = 0; j < NUM_THREADS; ++j) {
            ASSERT(value[j] == value[id]);
        }
        printf(LINE + id, COL, "Barrier thread %d: %d", do_getpid(),
               value[id]);
        barrier_wait(&barrier);
    }
    ASSERT(NUM_ITERATIONS == value[id]);
    printf(LINE + id, COL, "Barrier thread %d: Passed\t", do_getpid());
    do_exit();
}

void barrier1(void)
{
    barrier_thread(0);
}

void barrier2(void)
{
    barrier_thread(1);
}

void barrier3(void)
{
    barrier_thread(2);
}
