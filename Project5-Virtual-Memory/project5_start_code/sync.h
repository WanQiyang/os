/* Author(s): <Your name here>
 * COS 318, Fall 2013: Project 3 Pre-emptive Scheduler
 * Defines locks, condition variables, sempahores and barriers.
*/

#ifndef THREAD_H
#define THREAD_H

#include "queue.h"
#include "scheduler.h"

typedef struct {
    enum {
        UNLOCKED,
        LOCKED
    } status;
    node_t wait_queue;
    pcb_t* held_task;
} lock_t;

// Prototypes
// Lock functions
void lock_init(lock_t *);
int  lock_acquire(lock_t *); // return 0 on success, 1 on failure (extra credit)
void lock_release(lock_t *);

#endif
