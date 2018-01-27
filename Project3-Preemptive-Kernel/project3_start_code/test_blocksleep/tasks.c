/* tasks.c: the list of threads and processes to run (DO NOT CHANGE) */

#include "scheduler.h"
#include "th.h"

struct task_info task[] = {
    TH(&thread_1),
    TH(&thread_2)
    };

enum {
    NUM_TASKS = sizeof task / sizeof(struct task_info)
};
