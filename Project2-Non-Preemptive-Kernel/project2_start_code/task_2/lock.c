/* lock.c: mutual exclusion
 * If SPIN is false, lock_acquire() should block the calling process until its request can be satisfied
 * Your solution must satisfy the FIFO fairness property
 *
 *  It is assumed that only one process will attempt to initialize a lock,
 *                that processes will call lock_acquire only on an initialized lock
 *                that processes will call lock_release after lock_acquire (and before exiting)
 *                that processes will release a lock before attempting to reacquire it
 */

#include "common.h"
#include "lock.h"
#include "scheduler.h"

enum {
	SPIN = TRUE,
};

void lock_init(lock_t * l)
{
	if (SPIN) {
		l->status = UNLOCKED;
	} else {
		/* need student add */
	}
}

void lock_acquire(lock_t * l)
{
	if (SPIN) {
		while (LOCKED == l->status)
		{
			do_yield();
		}
		l->status = LOCKED;
	} else {
		/* need student add */
	}
}

void lock_release(lock_t * l)
{
	if (SPIN) {
		l->status = UNLOCKED;
	} else {
		/* need student add */
	}
}
