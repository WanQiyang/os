/* interrupt.c */
/* Do not change this file */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "util.h"
#include "interrupt.h"

#include "cp0regdefs.h"

#define EBASE 0xbfc00000
#define EBASE_OFFSET 0x380

enum ExcCode
{
     INT,
     MOD,
     TLBL,
     TLBS,
     ADEL,
     ADES,
     IBE,
     DBE,
     SYS,
     BP,
     RI,
     CPU,
     OV,
     TR,
     FPE=15,
     WATCH=23
};

uint32_t interrupt_handlers[32];
#define TIMER_INTERVAL 150000000
#define REFRESH_SCREEN_RATE 10

// defined in entry.S
extern void exception_handler_start();
extern void exception_handler_end();
extern void handle_int();
extern void handle_syscall();
extern void simple_handler();

void init_interrupts(void)
{
     int i = 0;
     for (i = 0; i < 32; ++i) {
          interrupt_handlers[i] = simple_handler;
     }
     interrupt_handlers[INT] = handle_int;
     interrupt_handlers[SYS] = handle_syscall;


     // fill nop
     bzero(EBASE,EBASE_OFFSET);
     // copy the exception handler to EBase
     bcopy(exception_handler_start,EBASE+EBASE_OFFSET,
           exception_handler_end-exception_handler_start);
     // When BEV=0, EBASE change to 0x80000000
     // offset change to 0x180
     bzero(0x80000000,0x180);
     bcopy(exception_handler_start,0x80000180,
           exception_handler_end-exception_handler_start);

     reset_timer(TIMER_INTERVAL); // 300MHz
     uint32_t cp0_status = get_cp0_status();

     cp0_status |= 0x8001;

     set_cp0_status(STATUS_CU0 | cp0_status);
}

void c_simple_handler(int ip_source, int exc_code)
{
    enter_critical();
}



/* This function handles executing a given syscall, and returns the result
   to syscall_entry in entry.S, from where it is returned to the calling
   process. Before we get here, syscall_entry() will have stored the
   context of the process making the syscall, and entered a critical
   section (through enter_critical()).
*/
void system_call_helper(int fn, int arg1, int arg2, int arg3)
{
    int ret_val = 0;

    ASSERT2(current_running->nested_count == 0,
            "A process/thread that was running inside the kernel made a syscall.");
    enter_critical();
    current_running->nested_count++;
    leave_critical();
    // printf(19, 10, "syscall: %d, nested_count: %d", fn, current_running->nested_count);

    // Call function and return result as usual (ie, "return ret_val");
    if (fn < 0 || NUM_SYSCALLS <= fn) {
        // Illegal system call number, call exit instead
        fn = SYSCALL_EXIT;
    }
    /* In C's calling convention, caller is responsible for cleaning up
       the stack. Therefore we don't really need to distinguish between
       different argument numbers. Just pass all 3 arguments and it will
       work */
    ret_val = syscall[fn] (arg1,arg2,arg3);

    // We can not leave the critical section we enter here before we
    // return in syscall_entry.
    // This is due to a potential race condition on a scratch variable
    // used by syscall_entry.
    enter_critical();
    current_running->nested_count--;
    current_running->user_tf.regs[2] = ret_val;
    current_running->user_tf.cp0_epc = current_running->user_tf.cp0_epc + 4;
    // printf(20, 10, "syscall: %d, nested_count: %d", fn, current_running->nested_count);
    ASSERT2(current_running->nested_count == 0, "Wrong nest count at B");

    // leave_critical() will be called during restoring context.
}
