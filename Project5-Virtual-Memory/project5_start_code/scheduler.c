/* Author(s): <Your name here>
 * COS 318, Fall 2013: Project 3 Pre-emptive Scheduler
 * Implementation of the process scheduler for the kernel.
 */

#include "common.h"
#include "interrupt.h"
#include "queue.h"
#include "printf.h"
#include "scheduler.h"
#include "util.h"
#include "syslib.h"

pcb_t *current_running;
node_t ready_queue;
node_t sleep_wait_queue;
// more variables...
volatile uint64_t time_elapsed;

/* TODO:wake up sleeping processes whose deadlines have passed */
void check_sleeping(){

     pcb_t *sleeping;

     while (!is_empty(&sleep_wait_queue)) {
          sleeping = (pcb_t *)peek(&sleep_wait_queue);
          if (sleeping->deadline <= time_elapsed) {
               sleeping->status = READY;
               enqueue(&ready_queue, dequeue(&sleep_wait_queue));
          } else {
               return;
          }
     }

}

/* Round-robin scheduling: Save current_running before preempting */
void put_current_running(){
     // TO-DO: Do we want to wake up sleeping processes here as well?
//  check_sleeping();
     current_running->status = READY;
     enqueue(&ready_queue, (node_t *)current_running);
}

/* Change current_running to the next task */
void scheduler(){
     ASSERT(disable_count);
     check_sleeping(); // wake up sleeping processes
     while (is_empty(&ready_queue)){
          leave_critical();
          enter_critical();
          check_sleeping();
     }
     current_running = (pcb_t *) dequeue(&ready_queue);
     // TODO: switch page table for process
     set_pt();

     ASSERT(NULL != current_running);
     ++current_running->entry_count;
}

int lte_deadline(node_t *a, node_t *b) {
     pcb_t *x = (pcb_t *)a;
     pcb_t *y = (pcb_t *)b;

     if (x->deadline <= y->deadline) {
          return 1;
     } else {
          return 0;
     }
}

void do_sleep(int milliseconds){
     ASSERT(!disable_count);

     enter_critical();
     // TODO: The timer is too slow. So I use `milliseconds/1000` here. So ugly.
     current_running->deadline = time_elapsed + milliseconds/1000;

     if(current_running->deadline > time_elapsed){
          current_running->status = SLEEPING;
          enqueue(&sleep_wait_queue, current_running);
          scheduler_entry();
          enter_critical();
     }
     leave_critical();
}

void do_yield(){
     enter_critical();
     put_current_running();
     scheduler_entry();
}

void do_exit(){
     enter_critical();
     current_running->status = EXITED;
     scheduler_entry();
     /* No need for leave_critical() since scheduler_entry() never returns */
}

void block(node_t * wait_queue){
     ASSERT(disable_count);
     current_running->status = BLOCKED;
     enqueue(wait_queue, (node_t *) current_running);
     scheduler_entry();
     enter_critical();
}

void unblock(pcb_t * task){
     ASSERT(disable_count);
     task->status = READY;
     enqueue(&ready_queue, (node_t *) task);
}

pid_t do_getpid(){
     pid_t pid;
     enter_critical();
     pid = current_running->pid;
     leave_critical();
     return pid;
}

uint64_t do_gettimeofday(void){
     return time_elapsed;
}

/* Extra credit */
priority_t do_getpriority(){
     priority_t priority;
     enter_critical();
     priority = current_running->priority;
     leave_critical();
     return priority;
}

/* Extra credit */
void do_setpriority(priority_t priority){
     if( priority >= 1 ){
          enter_critical();
          current_running->priority = priority;
          leave_critical();
     }
}

uint64_t get_timer(void) {
     return do_gettimeofday();
}
