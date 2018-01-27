
#include "common.h"
#include "interrupt.h"
#include "queue.h"
#include "scheduler.h"
#include "sync.h"
#include "util.h"
#include "printf.h"

/*TODO operate */
static bool_t unblock_one(node_t *wait_enqueue)
{
	ASSERT(current_running->nested_count);

	pcb_t *pcb = NULL;
	pcb = (pcb_t *)dequeue(wait_enqueue);
	if(pcb != NULL)
	{
		unblock(pcb);
		return TRUE;
	}
	return FALSE;
}

/* Initialize a lock */
void lock_init(lock_t * l){
    /* No critical section; it is the caller's responsibility to make sure
       that locks are initialized only once */
    l->status = UNLOCKED;
    queue_init(&l->wait_queue);
    l->held_task = NULL;
}

static int lock_acquire_helper(lock_t * l){
  ASSERT(disable_count);
  if (LOCKED == l->status) {
    current_running->blocking_lock = (void*)l;
    pcb_t* cur_task = current_running;
    lock_t* cur_lock;
    while (cur_task){
      pcb_t* tmp_task;
      cur_lock = (lock_t*)cur_task->blocking_lock;
      if (cur_lock){
        tmp_task = (pcb_t*)cur_lock->held_task;
        if (tmp_task==current_running){
          current_running->blocking_lock = NULL;
          return 1;
        }
      }
      else break;
      cur_task = tmp_task;
    }
    block(&l->wait_queue);
    current_running->blocking_lock = NULL;
  } else {
    l->status = LOCKED;
	l->held_task = (void*) current_running;
  }

  return 0;
}

// Return 0 on succes
// Return 1 on failure due to deadlock (extra credit)
int lock_acquire(lock_t * l){
    enter_critical();
    int result = lock_acquire_helper(l);
	ASSERT(disable_count);
    leave_critical();

    return result;
}

static void lock_release_helper(lock_t * l){
  ASSERT(disable_count);
  if (!unblock_one(&l->wait_queue)) {
    l->status = UNLOCKED;
  }
  l->held_task = NULL;

}

void lock_release(lock_t * l){
    enter_critical();
    lock_release_helper(l);
	ASSERT(disable_count);
    leave_critical();
}

/* TODO: Initialize a condition variable */
void condition_init(condition_t * c){
	enter_critical();
	queue_init(&c->wait_queue);
	ASSERT(disable_count);
	leave_critical();
}

/* TODO: Release lock m and block the thread (enqueued on c).  When unblocked,
   re-acquire m */
/* error */
void condition_wait(lock_t * m, condition_t * c){
	enter_critical();
	enqueue(&c->wait_queue, (node_t*)current_running);
	ASSERT(disable_count);
	leave_critical();
	lock_release(m);
	enter_critical();
	scheduler_entry();
	/* ASSERT(disable_count);
	leave_critical(); */
	lock_acquire(m);
}

/* TODO: Unblock the first thread waiting on c, if it exists */
void condition_signal(condition_t * c){
	enter_critical();
	node_t* pcb_wait;
	if (!is_empty(&c->wait_queue)) {
		pcb_wait = dequeue(&c->wait_queue);
		enqueue(&ready_queue, pcb_wait);
	}
	ASSERT(disable_count);
	leave_critical();
}

/* TODO: Unblock all threads waiting on c */
void condition_broadcast(condition_t * c){
	enter_critical();
	node_t* pcb_wait;
	while (!is_empty(&c->wait_queue)) {
		pcb_wait = dequeue(&c->wait_queue);
		enqueue(&ready_queue, pcb_wait);
	}
	ASSERT(disable_count);
	leave_critical();
}

/* TODO: Initialize a semaphore with the specified value. value must be >= 0 */
void semaphore_init(semaphore_t * s, int value){
	enter_critical();
	s->value = value;
	queue_init(&s->wait_queue);
	ASSERT(disable_count);
	leave_critical();
}

/* TODO: Increment the semaphore value atomically */
void semaphore_up(semaphore_t * s){
	enter_critical();
	node_t* pcb_wait;
	if (!is_empty(&s->wait_queue)) {
		pcb_wait = dequeue(&s->wait_queue);
		enqueue(&ready_queue, pcb_wait);
	}
	else {
		s->value++;
	}
	ASSERT(disable_count);
	leave_critical();
}

/* TODO: Block until the semaphore value is greater than zero and decrement it */
void semaphore_down(semaphore_t * s){
	enter_critical();
	if (s->value == 0) {
		enqueue(&s->wait_queue, (node_t*) current_running);
		scheduler_entry();
		enter_critical();
	}
	else {
		s->value--;
	}
	ASSERT(disable_count);
	leave_critical();
}

/* TODO: Initialize a barrier.  n is number of threads that rendezvous at the
   barrier */
void barrier_init(barrier_t * b, int n){
	enter_critical();
	b->n = b->value = n;
	queue_init(&b->wait_queue);
	ASSERT(disable_count);
	leave_critical();
}

/* TODO: Block until all n threads have called barrier_wait */
void barrier_wait(barrier_t * b){
	enter_critical();
	node_t* pcb_wait;
	if (b->value == 1) {
		while (!is_empty(&b->wait_queue)) {
			pcb_wait = dequeue(&b->wait_queue);
			enqueue(&ready_queue, pcb_wait);
		}
		b->value = b->n;
	}
	else {
		b->value--;
		enqueue(&b->wait_queue, (node_t*)current_running);
		scheduler_entry();
		enter_critical();
	}
	ASSERT(disable_count);
	leave_critical();
}
