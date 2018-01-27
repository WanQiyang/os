/* kernel.h: definitions used by kernel code */

#ifndef KERNEL_H
#define KERNEL_H

#define NUM_REGISTERS 8

#include "common.h"

/* ENTRY_POINT points to a location that holds a pointer to kernel_entry */
#define ENTRY_POINT ((void (**)(int)) 0xa080fff8)

/* System call numbers */
enum {
    SYSCALL_YIELD,
    SYSCALL_EXIT,
};

/* All stacks should be STACK_SIZE bytes large
 * The first stack should be placed at location STACK_MIN
 * Only memory below STACK_MAX should be used for stacks
 */
enum {
    STACK_MIN = 0xa1000000,
    STACK_SIZE = 0x10000,
    STACK_MAX = 0xa2000000,
};

typedef enum {
	PROCESS_BLOCKED,
	PROCESS_READY,
	PROCESS_RUNNING,
	PROCESS_EXITED,
} process_state;

#define REGISTER_NUM 32

typedef struct pcb {
	/* need student add */
  uint32_t task_type;
  void* mem_addr;
  uint32_t mem_size;
  uint32_t pid;
  process_state state;
  struct pcb *parent;
  uint32_t context[REGISTER_NUM];
  /* char name[16] */
} pcb_t;
/* The task currently running.  Accessed by scheduler.c and by entry.s assembly methods */
extern volatile pcb_t *current_running;

void kernel_entry(int fn);

#endif                          /* KERNEL_H */
