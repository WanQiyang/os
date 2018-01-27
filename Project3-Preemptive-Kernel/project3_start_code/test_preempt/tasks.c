/* tasks.c: the list of threads and processes to run (DO NOT CHANGE) */

#include "scheduler.h"
#include "th.h"

struct task_info task[] = {
    /* clock and status thread */
    TH(&clock_thread),
    TH(&ps_thread),
    PROC(PROC2_ADDR),
    PROC(PROC1_ADDR),
    };

enum {
    NUM_TASKS = sizeof task / sizeof(struct task_info)
};
