/* tasks.c: the list of threads and processes to run (DO NOT CHANGE) */

#include "scheduler.h"
#include "th.h"


struct task_info task[] = {
  /* clock and status thread */
  TH(&clock_thread),
  /* lock and condition variable test threads */
//  TH(&thread2), TH(&thread3),
  /* dining philosopher threads, named for the LEDs they used to
     trigger, before I (David Eisenstat) figured out that they were
     causing the keyboard controller to choke */
//  TH(&num), TH(&caps), TH(&scroll_th),
  /* barrier test threads */
//  TH(&barrier1), TH(&barrier2), TH(&barrier3),
  /* plane process */
  PROC(PROC1_ADDR),
  /* calculation process */
  PROC(PROC2_ADDR)
};

enum {
    NUM_TASKS = sizeof task / sizeof(struct task_info)
};
