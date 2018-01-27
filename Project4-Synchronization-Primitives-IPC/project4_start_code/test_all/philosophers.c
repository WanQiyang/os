#include "common.h"
#include "scheduler.h"
#include "sync.h"
#include "util.h"
#include "printf.h"

/* Dijkstra's dining philosophers */

enum {
    NUM_PHILOSOPHERS = 3,
    MIN_THINK_TIME = 5000,
    MAX_THINK_TIME = 10000,
    MIN_EAT_TIME = 2000,
    MAX_EAT_TIME = 4000,
    FIRST_LINE = 16,            /* Line where the result should be printed 
                                 */
    COL = 48                    /* Column where the result should be
                                   printed */
};

static volatile bool_t eating[NUM_PHILOSOPHERS];        /* initialized to
                                                           FALSE for us */
static semaphore_t fork[NUM_PHILOSOPHERS];
static bool_t forks_initialized = FALSE;

static void status(int id, char *s)
{
    printf(FIRST_LINE + id, COL, "Philosopher %d: %s", do_getpid(), s);
}

static void philosopher(int id)
{
    if (0 == id) {
        int i;

        srand(get_timer());
        for (i = 0; i < NUM_PHILOSOPHERS; ++i) {
            semaphore_init(&fork[i], 1);
        }
        forks_initialized = TRUE;
    } else {
        while (!forks_initialized) {
            do_yield();
        }
    }

    while (TRUE) {
        status(id, "thinking");
        do_sleep(MIN_THINK_TIME +
                 rand() % (MAX_THINK_TIME - MIN_THINK_TIME));
        status(id, "hungry  ");
        if (0 == id) {
            semaphore_down(&fork[0]);
            semaphore_down(&fork[NUM_PHILOSOPHERS - 1]);
        } else {
            semaphore_down(&fork[id - 1]);
            semaphore_down(&fork[id]);
        }
        eating[id] = TRUE;
        status(id, "eating  ");
        do_sleep(MIN_EAT_TIME + rand() % (MAX_EAT_TIME - MIN_EAT_TIME));
        ASSERT(!eating[(id + 1) % NUM_PHILOSOPHERS]);
        ASSERT(!eating[(id - 1 + NUM_PHILOSOPHERS) % NUM_PHILOSOPHERS]);
        eating[id] = FALSE;
        if (0 == id) {
            semaphore_up(&fork[0]);
            semaphore_up(&fork[NUM_PHILOSOPHERS - 1]);
        } else {
            semaphore_up(&fork[id - 1]);
            semaphore_up(&fork[id]);
        }
    }
}

void num(void)
{
    philosopher(0);
}

void caps(void)
{
    philosopher(1);
}

void scroll_th(void)
{
    philosopher(2);
}
