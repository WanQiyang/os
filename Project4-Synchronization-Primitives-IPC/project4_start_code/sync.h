/* Author(s): <Your name here>
 * COS 318, Fall 2013: Project 3 Pre-emptive Scheduler
 * Defines locks, condition variables, sempahores and barriers.
*/

#ifndef THREAD_H
#define THREAD_H

#include "common.h"

/* TODO */
typedef struct condition{
    node_t wait_queue;
} condition_t;

/* TODO */
typedef struct semaphore{
    int value;
    node_t wait_queue;
} semaphore_t;

/* TODO */
typedef struct barrier{
    int n;
    int value;
    node_t wait_queue;
} barrier_t;

// Prototypes
// Lock functions
void lock_init(lock_t *lock);
int  lock_acquire(lock_t *lock); // return 0 on success, 1 on failure (extra credit)
void lock_release(lock_t *lock);

// Initialize c
void condition_init(condition_t * c);

// Unlock m and block on condition c, when unblocked acquire lock
// m
void condition_wait(lock_t * m, condition_t * c);

// Unblock first thread enqueued on c
void condition_signal(condition_t * c);

// Unblock all threads enqueued on c
void condition_broadcast(condition_t * c);

// Semaphore functions
void semaphore_init(semaphore_t * s, int value);
void semaphore_up(semaphore_t * s);
void semaphore_down(semaphore_t * s);

// Initialize a barrier, to block all processes entering the
// barrier until N processes have entered
void barrier_init(barrier_t * b, int n);

// Make calling process wait at barrier until all N processes have
// called barrier_wait()
void barrier_wait(barrier_t * b);

#endif
