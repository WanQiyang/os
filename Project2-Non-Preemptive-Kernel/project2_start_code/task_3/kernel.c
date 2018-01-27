/*
   kernel.c
   the start of kernel
   */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "th.h"
#include "util.h"
#include "queue.h"

#include "tasks.c"

volatile pcb_t *current_running;

queue_t ready_queue, blocked_queue;
pcb_t *ready_arr[NUM_TASKS];
pcb_t *blocked_arr[NUM_TASKS];

struct queue ready_q, blocked_q;
pcb_t task_arr[NUM_TASKS];

/*
   this function is the entry point for the kernel
   It must be the first function in the file
   */

#define PORT3f8 0xbfe48000

 void printnum(unsigned long long n)
 {
   int i,j;
   unsigned char a[40];
   unsigned long port = PORT3f8;
   i=10000;
   while(i--);

   i = 0;
   do {
   a[i] = n % 16;
   n = n / 16;
   i++;
   }while(n);

  for (j=i-1;j>=0;j--) {
   if (a[j]>=10) {
      *(unsigned char*)port = 'a' + a[j] - 10;
    }else{
	*(unsigned char*)port = '0' + a[j];
   }
  }
  printstr("\r\n");
}

void _stat(void){

	/* some scheduler queue initialize */
	/* need student add */

  ready_queue = & ready_q;
  blocked_queue = & blocked_q;

  queue_init(ready_queue);
  ready_queue->pcbs = (pcb_t**) ready_arr;
  ready_queue->capacity = NUM_TASKS;
  queue_init(blocked_queue);
  blocked_queue->pcbs = (pcb_t**) blocked_arr;
  blocked_queue->capacity = NUM_TASKS;

	clear_screen(0, 0, 30, 24);

	/* Initialize the PCBs and the ready queue */
	/* need student add */

  pcb_t *task_pcb;
  int i, j;
  for(i=0;i<NUM_TASKS;i++) {
    task_pcb = task_arr + i;
    task_pcb->task_type = task[i]->task_type;
    task_pcb->mem_addr = task[i]->entry_point;
    task_pcb->mem_size = 0x10000;
    task_pcb->pid = i;
    task_pcb->state = PROCESS_READY;
    task_pcb->parent = NULL;
    for(j=0;j<REGISTER_NUM;j++)
      task_pcb->context[j] = 0;

    task_pcb->context[29] = STACK_MIN + (i + 1) * STACK_SIZE;
    ASSERT(task_pcb->context[29] < STACK_MAX);
    task_pcb->context[31] = task_pcb->mem_addr;
    /* print_hex(1, 1, task_pcb->context[31]);
    printnum(task_pcb->context[29]);
    printnum(task_pcb->context[31]); */

    queue_push(ready_queue, task_pcb);
  }

	/*Schedule the first task */
  /* current_running = queue_pop(ready_queue); */
  /* current_running->state = PROCESS_RUNNING; */
	scheduler_count = 0;
	scheduler_entry();

	/*We shouldn't ever get here */
	ASSERT(0);
}
