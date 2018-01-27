/* tasks.c: the list of threads and processes to run (DO NOT CHANGE) */

#include "scheduler.h"
#include "th.h"


struct task_info task[] = {
  /* clock and status thread */
  TH(&clock_thread),
  /* plane process */
  PROC(PROC1_ADDR, PROC1_LOC, PROC1_SIZE),
  /* calculation process */
  PROC(PROC2_ADDR, PROC2_LOC, PROC2_SIZE),
  /* Page test process */
  // PROC(PROC3_ADDR, PROC3_LOC, PROC3_SIZE)
};

enum {
    NUM_TASKS = sizeof task / sizeof(struct task_info)
};
