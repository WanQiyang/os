/* Simple counter program, to check the functionality of do_yield(). Print
   time in seconds.

   Best viewed with tabs set to 4 spaces. */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "th.h"
#include "util.h"
#include "printf.h"

/*
 * This thread runs indefinitely, which means that the
 * scheduler should never run out of processes.
 */
void thread_1(void)
{
    priority_t priority = 4;
    do_setpriority(priority);
    printf(2, 10, "Priority of thread 1 : %d", do_getpriority());
    int i=0;
    while (TRUE) {
        i++;
        printf(3, 10, "Thread 1 : %d", i);
        do_yield();
    }
}

void thread_2(void)
{
    priority_t priority = 2;
    do_setpriority(priority);
    printf(5, 10, "Priority of thread 2 : %d", do_getpriority());
    int i=0;
    while (TRUE) {
        i++;
        printf(6, 10, "Thread 2 : %d", i);
        do_yield();
    }
}

void thread_3(void)
{
    priority_t priority = 1;
    do_setpriority(priority);
    printf(8, 10, "Priority of thread 3 : %d", do_getpriority());
    int i=0;
    while (TRUE) {
        i++;
        printf(9, 10, "Thread 3 : %d", i);
        do_yield();
    }
}

void thread_4(void)
{
    priority_t priority = 3;
    do_setpriority(priority);
    printf(11, 10, "Priority of thread 4 : %d", do_getpriority());
    int i=0;
    while (TRUE) {
        i++;
        printf(12, 10, "Thread 4 : %d", i);
        do_yield();
    }
}
