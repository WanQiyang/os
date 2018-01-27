/* scheduler.c */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "util.h"
#include "queue.h"

int scheduler_count;
// process or thread runs time
uint64_t cpu_time;

void printstr(char *s);
void printnum(unsigned long long n);
void scheduler(void)
{
	++scheduler_count;

	// pop new pcb off ready queue
	/* need student add */
  if(!ready_queue->isEmpty) {
    current_running = queue_pop(ready_queue);
    current_running->state = PROCESS_RUNNING;
  }
  else {
    printstr("INFO: No more tasks!");
    ASSERT(0);
  }
}

void do_yield(void)
{
  __asm__ __volatile__("addiu $29, $29, -8\n\tsw $31, 28($29)");
	save_pcb();
	/* push the qurrently running process on ready queue */
	/* need student add */
  current_running->state = PROCESS_READY;
  queue_push(ready_queue, current_running);
  // current_running = queue_pop(ready_queue);
  // current_running->state = PROCESS_RUNNING;

	// call scheduler_entry to start next task
	scheduler_entry();

	// should never reach here
	ASSERT(0);
}

void do_exit(void)
{
	/* need student add */
  current_running->state = PROCESS_EXITED;
  scheduler_entry();
}

void block(void)
{
  /* uint32_t empty[2] = {1, 2}; */
  __asm__ __volatile__("addiu $29, $29, -8\n\tsw $31, 28($29)");
	save_pcb();
	/* need student add */
  current_running->state = PROCESS_BLOCKED;
  queue_push(blocked_queue, current_running);
  scheduler_entry();

	// should never reach here
	ASSERT(0);
}

int unblock(void)
{
	/* need student add */
 //  __asm__ __volatile__("addiu $29, $29, -8\n\tsw $31, 24($29)");
  save_pcb();

  current_running->state = PROCESS_READY;
  queue_push(ready_queue, current_running);
  if(blocked_tasks()) {
    current_running = queue_pop(blocked_queue);
    current_running->state = PROCESS_READY;
    queue_push(ready_queue, current_running);
  }
  
  scheduler_entry();

  ASSERT(0);
}

bool_t blocked_tasks(void)
{
	return !blocked_queue->isEmpty;
}
