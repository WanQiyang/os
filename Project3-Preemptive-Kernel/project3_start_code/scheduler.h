/* Author(s): <Your name here>
 * COS 318, Fall 2013: Project 3 Pre-emptive Scheduler
 * Defines the process scheduler for the kernel.
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "common.h"
#include "queue.h"

typedef enum {
     KERNEL_THREAD,
     PROCESS
} task_type_t;

typedef int priority_t;

typedef enum {
     FIRST_TIME,
     READY,
     BLOCKED,
     EXITED,
     SLEEPING
} status_t;

/* Simplified structure, used
 * in tasks.c to declare all
 * processes to start.
 */
struct task_info {
     uint32_t entry_point;
     task_type_t task_type;
};

/* Two macros for initializing a task_info structure */
#define TH(entry)   { .entry_point = (uint32_t) entry,	\
               .task_type = KERNEL_THREAD }
#define PROC(entry) { .entry_point = (uint32_t) entry,	\
               .task_type = PROCESS }

typedef struct Trapframe {
     /* Saved main processor registers. */
     uint32_t regs[32];

     /* Saved special registers. */
     uint32_t cp0_status;
     uint32_t hi;
     uint32_t lo;
     uint32_t cp0_badvaddr;
     uint32_t cp0_cause;
     uint32_t cp0_epc;
     uint32_t pc;
}trapframe_t;

/* Process control block */
typedef struct pcb {
	/* entry.S */
     node_t node;
	 trapframe_t kernel_tf;
	 trapframe_t user_tf;

     int nested_count;
     /* Ok, you may change the order from now on*/
     uint32_t entry_point;
     pid_t pid;
     task_type_t task_type;
     status_t status;
     uint32_t entry_count; // how many times this task has been pre-emtped to
     uint32_t deadline;

     void* blocking_lock;
} pcb_t;

extern node_t ready_queue;
extern node_t sleep_wait_queue;
extern pcb_t *current_running;
extern volatile uint32_t time_elapsed;

#define MAX_PID 1024
#define MAX_PRIORITY 5
extern priority_t my_priority[MAX_PID];
extern priority_t now_priority[MAX_PID];

/* Save the context and the kernel stack before calling scheduler.  This
   function is implemented in entry.S */
extern void scheduler_entry(void);

/* Schedule another task.  Call from a kernel thread or
   kernel_entry_helper() */
void do_yield(void);

/* Schedule another task.  Do not reschedule the current one.  Call from a
   kernel thread or kernel_entry_helper() */
void do_exit(void);

/* Return the current task's pid */
pid_t do_getpid(void);

/* Milliseconds since boot */
uint32_t do_gettimeofday(void);

/* Sleep for specified number of mulliseconds */
void do_sleep(int milliseconds);

/* Schedule another task, putting the current one in the specified queue.
   Do not reschedule the current task until it is unblocked */
void block(node_t * wait_queue);

/* Unblock the specified task */
void unblock(pcb_t * task);

/* Extra Credit */
/* Return the current task's priority */
priority_t do_getpriority(void);

/* Extra Credit */
/* Set the current task's priority */
void do_setpriority(priority_t priority);

#endif                          /* SCHEDULER_H */
