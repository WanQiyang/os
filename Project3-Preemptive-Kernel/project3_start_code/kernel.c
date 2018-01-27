/* kernel.c: Kernel with pre-emptive scheduler */
/* Do not change this file */

#include "common.h"
#include "interrupt.h"
#include "kernel.h"
#include "queue.h"
#include "scheduler.h"
#include "util.h"
#include "printf.h"
#include "printk.h"

#include "tasks.c"

#define NUM_PCBS      (NUM_TASKS)
#define INITIAL_PRIORITY 2

pcb_t pcb[NUM_PCBS];

/* This is the system call table, used in interrupt.c */
int (*syscall[NUM_SYSCALLS]) ();

static uint32_t *stack_new(void);
// static void first_entry(void);
static int invalid_syscall(void);
static void init_syscalls(void);
static void init_serial(void);
static void initialize_pcb(pcb_t *p, pid_t pid, struct task_info *ti);

extern void asm_start();

void printcharc(char ch);

void __attribute__((section(".entry_function"))) _start(void)
{
     asm_start();
     static pcb_t garbage_registers;
     int i;

     clear_screen(0, 0, 80, 30);

	 /* TODO: initialize queue */
     queue_init(&ready_queue);
     queue_init(&sleep_wait_queue);

     current_running = &garbage_registers;

     /* Transcribe the task[] array (in tasks.c)
      * into our pcb[] array.
      */
     for (i = 0; i < NUM_TASKS; ++i) {
        /* TODO: need add */
        initialize_pcb(&pcb[i], i, &task[i]);
     }

     init_syscalls();
     init_interrupts();

     //init_serial();
     /* Initialize random number generator */
     srand(get_timer()); /* using a random value */
     time_elapsed = 0;
     // now_priority = 0;

     /* Schedule the first task */
     enter_critical();
     scheduler_entry();
     /* We shouldn't ever get here */
     ASSERT(FALSE);
}


static uint32_t *stack_new()
{
     static uint32_t next_stack = 0xa0f00000;

     next_stack += 0x10000;
     ASSERT(next_stack <= 0xa1000000);
     return (uint32_t *) next_stack;
}

static void initialize_pcb(pcb_t *p, pid_t pid, struct task_info *ti)
{
  	 /* TODO: need add */
     bzero(&p->kernel_tf, sizeof(p->kernel_tf));
     bzero(&p->user_tf, sizeof(p->user_tf));
     // uint32_t cp0_status = get_cp0_status() | 0x10008000;
     uint32_t cp0_status = 0x10008000;
     // printf(20, 10, "cp0_status: %d", cp0_status);

     p->kernel_tf.regs[29] = (uint32_t) stack_new();
     p->kernel_tf.cp0_status = cp0_status;
     p->kernel_tf.regs[31] = (uint32_t) & first_entry;

     /* if(ti->task_type == KERNEL_THREAD) {
         p->user_tf.regs[29] = p->kernel_tf.regs[29];
     }
     else {
         p->user_tf.regs[29] = (uint32_t) stack_new();
     } */
     p->user_tf.regs[29] = p->kernel_tf.regs[29];
     p->user_tf.cp0_epc = ti->entry_point;
     p->user_tf.cp0_status = cp0_status;

     p->nested_count = (ti->task_type == KERNEL_THREAD);
     p->entry_point = ti->entry_point;
     p->pid = pid;
     p->task_type = ti->task_type;
     p->status = FIRST_TIME;
     p->entry_count = 0;
     p->deadline = 0;
     p->blocking_lock = NULL;
     my_priority[pid] = INITIAL_PRIORITY;
     now_priority[pid] = INITIAL_PRIORITY;
     enqueue(&ready_queue, (node_t *)p);
}



static int invalid_syscall(void)
{
     HALT("Invalid system call");
     /* Never get here */
     return 0;
}


/* Called by kernel to assign a system call handler to the array of
   system calls. */
static void init_syscalls()
{
     int fn;

     for (fn = 0; fn < NUM_SYSCALLS; ++fn) {
          syscall[fn] = &invalid_syscall;
     }
     syscall[SYSCALL_YIELD] = (int (*)()) &do_yield;
     syscall[SYSCALL_EXIT] = (int (*)()) &do_exit;
     syscall[SYSCALL_GETPID] = &do_getpid;
     syscall[SYSCALL_GETPRIORITY] = &do_getpriority;
     syscall[SYSCALL_SETPRIORITY] = (int (*)()) &do_setpriority;
     syscall[SYSCALL_SLEEP] = (int (*)()) &do_sleep;
     syscall[SYSCALL_SHUTDOWN] = (int (*)()) &do_shutdown;
     syscall[SYSCALL_WRITE_SERIAL] = (int (*)()) &do_write_serial;
     syscall[SYSCALL_PRINT_CHAR] = (int (*)()) &print_char;
}

/* Used for debugging */
void print_status(void)
{
     static char *status[] = { "First  ", "Ready", "Blocked", "Exited ", "Sleeping" };
     int i, base;

     base = 13;
     printf(base - 4, 6, "P R O C E S S   S T A T U S");
     printf(base - 2, 1, "Pid\tType\tStatus\tEntries");
     for (i = 0; i < NUM_PCBS && (base + i) < 25; i++) {
          printf(base + i, 1, "%d\t%s\t%s\t%d", pcb[i].pid,
                 pcb[i].task_type == KERNEL_THREAD ? "Thread" : "Process",
                 status[pcb[i].status], pcb[i].entry_count);
     }
}

void do_shutdown(void)
{
     /* These numbers will work for bochs
      * provided it was compiled WITH acpi.
      * the default ubuntu 9 version of bochs
      * is NOT compiled with acpi support, though
      * the version in the Friend center lab DO have it.
      * This will probably not work with
      * any real computer.
      */
     //outw( 0xB004, 0x0 | 0x2000 );

     /* Failing that... */
     HALT("Shutdown");
}

#define PORT3f8 0xbfe48000
#define PORT3fd 0xbfe48006

/* Write a byte to the 0-th serial port */
void do_write_serial(int character)
{

     // wait until port is free
     unsigned long port = PORT3f8;
     int i = 50000;
     while (i--);
     *(unsigned char*)port = character;

     //leave_critical();
}

void printcharc(char ch)
{
     do_write_serial(ch);
}

int print_char(int line, int col, char c){
	unsigned long port = PORT3f8;
	print_location(line, col);

	*(unsigned char *)port = c;

}
