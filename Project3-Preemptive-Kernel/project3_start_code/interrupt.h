/* interrupt.h */
/* Do not change this file */

#ifndef INTERRUPT_H
#define INTERRUPT_H

extern uint32_t interrupt_handlers[32];
void init_interrupts(void);
// Prototypes

// Copy the segment descriptor index "seg" into the DS and ES
// registers
void load_data_segments(int seg);

// Helper function for system calls
void system_call_helper(int fn, int arg1, int arg2, int arg3);

// Handler for fake irqs
void fake_irq7(void);

// Mask/unmask a hardware interrupt source
void mask_hw_int(int irq);
void unmask_hw_int(int irq);

/* Used for any other interrupt which should not happen */
void bogus_interrupt(void);

// The following prototypes are for functions located in entry.S
extern int32_t disable_count;
extern void reset_timer(int interval);

void syscall_entry(void);
void c_simple_handler(int ip_source, int exc_code);

/* Entry points for the above interrupt handlers (irq0 doesn't have an
   explicit interrupt handler, it calls the scheduler directly.) */
void irq0_entry(void);
void fake_irq7_entry(void);

// Enter/leave a critical region
void enter_critical(void);
void leave_critical(void);

#endif
