/* interrupt.c */
/* Do not change this file */

#include "common.h"
#include "kernel.h"
#include "memory.h"
#include "printf.h"
#include "scheduler.h"
#include "util.h"
#include "interrupt.h"

#include "cp0regdefs.h"

#define EBASE 0xbfc00000
#define EBASE_OFFSET 0x380

int line = 0;

enum ExcCode {
  INT,
  MOD,  // 1. tlb modify
  TLBL, // 2. tlb miss
  TLBS, // 3. tlb refill
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
  FPE = 15,
  WATCH = 23
};

uint32_t interrupt_handlers[32];
#define TIMER_INTERVAL 150000000
#define REFRESH_SCREEN_RATE 10

// defined in entry.S
extern void exception_handler_start();
extern void exception_handler_end();
extern void handle_int();
extern void handle_tlb();
extern void handle_syscall();
extern void simple_handler();

void init_interrupts(void) {
  int i = 0;
  for (i = 0; i < 32; ++i) {
    interrupt_handlers[i] = simple_handler;
  }
  interrupt_handlers[INT] = handle_int;
  interrupt_handlers[SYS] = handle_syscall;
  interrupt_handlers[TLBL] = handle_tlb;
  interrupt_handlers[TLBS] = handle_tlb;

  for (i = 0; i < 32; ++i) {
    /* printk("handler #%d => %x\n",i,interrupt_handlers[i]); */
  }

  // printk("exception_start : %x\n", exception_handler_start);
  // printk("exception_end : %x\n", exception_handler_end);
  // fill nop
  bzero(EBASE, EBASE_OFFSET);
  // copy the exception handler to EBase
  bcopy(exception_handler_start, EBASE + EBASE_OFFSET,
        exception_handler_end - exception_handler_start);
  // When BEV=0, EBASE change to 0x80000000
  // offset change to 0x180
  bzero(0x80000000, 0x180);
  bcopy(exception_handler_start, 0x80000180,
        exception_handler_end - exception_handler_start);

  reset_timer(TIMER_INTERVAL); // 300MHz
  uint32_t cp0_status = get_cp0_status();
  // printk("cp0_status: %x\n", cp0_status);
  cp0_status |= 0x8001;
  // printk("cp0_status: %x\n", STATUS_CU0 | cp0_status);
  set_cp0_status(STATUS_CU0 | cp0_status);
}

void c_simple_handler(int ip_source, int exc_code) {
  // return;
  print_str(1, 1, " ");
  printk("interrupt handler not implemented for #%d:%d\n", ip_source, exc_code);
  enter_critical();
  printk("PID:%d \t Registers:\n", current_running->pid);
  printk("CP0_STATUS:%08x \t hi:%08x \t lo:%08x\n",
         current_running->user_tf.cp0_status, current_running->user_tf.hi,
         current_running->user_tf.lo);
  printk("BADVADDR:%08x \t CAUSE:%08x \t EPC:%08x \t PC:%08x \n",
         current_running->user_tf.cp0_badvaddr,
         current_running->user_tf.cp0_cause,
         current_running->user_tf.cp0_epc, current_running->user_tf.pc);
  printk("zero:%08x \t at:%08x \t v0:%08x \t v1:%08x \n",
         current_running->user_tf.regs[0], current_running->user_tf.regs[1],
         current_running->user_tf.regs[2],
         current_running->user_tf.regs[3]);
  printk("a0:%08x \t a1:%08x \t a2:%08x \t a3:%08x \n",
         current_running->user_tf.regs[4], current_running->user_tf.regs[5],
         current_running->user_tf.regs[6],
         current_running->user_tf.regs[7]);
  printk("t0:%08x \t t1:%08x \t t2:%08x \t t3:%08x \n",
         current_running->user_tf.regs[8], current_running->user_tf.regs[9],
         current_running->user_tf.regs[10],
         current_running->user_tf.regs[11]);
  printk(
      "t4:%08x \t t5:%08x \t t6:%08x \t t7:%08x \n",
      current_running->user_tf.regs[12], current_running->user_tf.regs[13],
      current_running->user_tf.regs[14], current_running->user_tf.regs[15]);
  printk(
      "s0:%08x \t s1:%08x \t s2:%08x \t s3:%08x \n",
      current_running->user_tf.regs[16], current_running->user_tf.regs[17],
      current_running->user_tf.regs[18], current_running->user_tf.regs[19]);
  printk(
      "s4:%08x \t s5:%08x \t s6:%08x \t s7:%08x \n",
      current_running->user_tf.regs[20], current_running->user_tf.regs[21],
      current_running->user_tf.regs[22], current_running->user_tf.regs[23]);
  printk(
      "t8:%08x \t t9:%08x \t k0:%08x \t k1:%08x \n",
      current_running->user_tf.regs[24], current_running->user_tf.regs[25],
      current_running->user_tf.regs[26], current_running->user_tf.regs[27]);
  printk(
      "gp:%08x \t sp:%08x \t fp:%08x \t ra:%08x \n",
      current_running->user_tf.regs[28], current_running->user_tf.regs[29],
      current_running->user_tf.regs[30], current_running->user_tf.regs[31]);
}

extern char screen[];
extern int need_refresh;

static void refresh_screen() {
  printcharc('');
  printcharc('[');
  printcharc('2');
  printcharc('J');

  int i, j;
  for (i = 0; i < 30; ++i) {
    for (j = 0; j < 80; ++j) {
      printcharc(screen[i * 80 + j]);
    }
    printcharc('\n');
    printcharc('\r');
  }
}

void timer_irq() {
  enter_critical();
  time_elapsed++;

  reset_timer(TIMER_INTERVAL);

  if (current_running->nested_count)
    return;
  put_current_running();
  scheduler_entry();
  enter_critical();
}

// The following three variables are used by the dummy exception handlers.

/* This function handles executing a given syscall, and returns the result
   to syscall_entry in entry.S, from where it is returned to the calling
   process. Before we get here, syscall_entry() will have stored the
   context of the process making the syscall, and entered a critical
   section (through enter_critical()).

   Note: The use of leave_critical() causes the interrupts to be turned on
   again after leave_critical. (cr->disable_count goes from 1 to 0 again.)

   This makes sense if we want system calls or other interrupt handlers to
   be interruptable (for instance allowing a timer interrupt to preempt a
   process while it's inside the kernel in a system call).

   It does, however, also mean that we can get interrupts while we are
   inside another interrupt handler (the same thing is done in the other
   interrupt handlers).

   In syslib.c we put systemcall number in eax and arg1 in ebx. The return
   value is returned in eax (as usual).

   Before entering the processor has switched to the kernel stack (PMSA p.
   209, Privilege level switch without error code) */
void system_call_helper(int fn, int arg1, int arg2, int arg3) {
  int ret_val = 0;

  ASSERT2(
      current_running->nested_count == 0,
      "A process/thread that was running inside the kernel made a syscall.");
  enter_critical();
  current_running->nested_count++;
  leave_critical();

  // Call function and return result as usual (ie, "return ret_val");
  if (fn < 0 || NUM_SYSCALLS <= fn) {
    // Illegal system call number, call exit instead
    fn = SYSCALL_EXIT;
  }
  /* In C's calling convention, caller is responsible for cleaning up
     the stack. Therefore we don't really need to distinguish between
     different argument numbers. Just pass all 3 arguments and it will
     work */
  ret_val = syscall[fn](arg1, arg2, arg3);

  // We can not leave the critical section we enter here before we
  // return in syscall_entry.
  // This is due to a potential race condition on a scratch variable
  // used by syscall_entry.
  enter_critical();
  current_running->nested_count--;
  current_running->user_tf.regs[2] = ret_val;
  current_running->user_tf.cp0_epc = current_running->user_tf.cp0_epc + 4;
  ASSERT2(current_running->nested_count == 0, "Wrong nest count at B");

  // leave_critical() will be called during restoring context.
}

/* Disable a hardware interrupt source by telling the controller to ignore
   it.

   NOTE: each thread/ process has its own interrupt mask, so this will
   only mask irqI for current_running. The other threads/processes can
   still get an irq <irq> request.

   irq is interrupt number 0-7 */
void mask_hw_int(int irq) {
  unsigned char mask;

  // Read interrupt mask register
  // mask = inb(0x21);
  // Disable <irq> by or'ing the mask with the corresponding bit
  mask |= (1 << irq);
  // Write interrupt mask register
  // outb(0x21, mask);
}

// Unmask the hardware interrupt source indicated by irq
void unmask_hw_int(int irq) {
  unsigned char mask;

  // mask = inb(0x21);
  mask &= ~(1 << irq);
  // outb(0x21, mask);
}

/* Interrupt generated on IRQ line 7 when the timer is working at a very
   high frequency. I (LAB) only get fake_irq7 when timer 0 has a frequency
   of about 0.1 MHZ. At 0.1 MHz, the time-slice is so short that we cannot
   do any useful work. Note that this function does not save and restore
   the context, some register will probably be destroyed. See Page 1007,
   Chapter 17, Warnings section in The Undocumented PC */
void fake_irq7(void) {
#ifdef DEBUG
  static int fake_irq7_count = 0;
  static int iis_flag;

  /* Read Interrupt In-Service Register */
  // outb(0x20, 0x0b);
  // iis_flag = inb(0x20);

  /* ASSERT2(IRQ 7 not in-service) */
  ASSERT2((iis_flag & (1 << 7)) == 0, "Real irq7 !");
  print_str(2, 0, "Fake irq7 count : ");
  print_int(2, 20, ++fake_irq7_count);
#endif
}

void handle_tlb_c(void)
{
    static int tlb_refill_count = 0;
    static int tlb_invalid_count = 0;

    enter_critical();
    // printk("Handle TLB requirement!!!\n");
    /*while (1) {
    ;
    }*/
    uint32_t entryhi = get_cp0_entryhi();
    uint32_t badvpn2 = entryhi >> 13;
    uint32_t badvaddr = get_cp0_badvaddr();
    uint32_t entrylo_offset = (badvaddr & 0x1000) >> 12;
    ASSERT2((badvpn2 << 3) < PAGE_SIZE, "Virtual Address Overflow !");
    uint32_t* p_pte = (uint32_t*)(current_running->page_table + (badvpn2 << 3));
/*
    asm volatile("tlbp");
    if((signed)get_cp0_index() < 0) {
        set_cp0_entrylo0(*(p_pte));
        set_cp0_entrylo1(*(p_pte + 1));
        asm volatile("tlbwr");
    } else {
        ASSERT2(FALSE, "TLB Invalid !");
        int free_index;
        uint32_t pte;
        if(get_cp0_entrylo0() & PE_V == 0) {
            free_index = page_alloc(0);
            pte = (page_paddr(free_index) >> 6) & 0xffffffc0 | PE_UC | PE_V;
            *p_pte = pte;
        }
        if(get_cp0_entrylo1() & PE_V == 0) {
            free_index = page_alloc(0);
            pte = (page_paddr(free_index) >> 6) & 0xffffffc0 | PE_UC | PE_V;
            *(p_pte + 1) = pte;
        }
        tlb_flush(entryhi);
    }
*/
    asm volatile("tlbp");
    if((signed)get_cp0_index() >= 0) {
        tlb_invalid_count++;
        // ASSERT2(FALSE, "TLB Invalid !");
        int free_index;
        uint32_t v_start, l_start, offset, old_pte, pte;
        // print_str(22, 1, " ");
        // printk("badvpn2: %x, lo0: %x, lo1: %x .", badvpn2, get_cp0_entrylo0(), get_cp0_entrylo1());
        /*if((get_cp0_entrylo0() & PE_V) == 0) {
            free_index = page_alloc(0);
            old_pte = *(p_pte);
            pte = ((page_paddr(free_index) >> 6) & 0xffffffc0) | PE_UC | PE_D | PE_V;
            *p_pte = pte;
            set_cp0_entrylo0(*(p_pte));
            asm volatile("tlbwi");
            if(old_pte & 0x80000000) {
                bcopy(swap_vaddr((old_pte & 0x7fffffff) >> 6), page_vaddr(free_index), PAGE_SIZE);
            } else {
                v_start = current_running->entry_point & 0xffffe000;
                l_start = current_running->loc - (current_running->entry_point - v_start);
                offset = (badvpn2 << 13) - v_start;
                if(offset < current_running->size + (current_running->entry_point - v_start))
                    bcopy(l_start + offset, page_vaddr(free_index), PAGE_SIZE);
            }
            print_str(18, 1, " ");
            printk("Invalid0 : %x, pte: %x", badvpn2, pte);
        }
        if((get_cp0_entrylo1() & PE_V) == 0) {
            free_index = page_alloc(0);
            old_pte = *(p_pte + 1);
            set_cp0_entrylo1(*(p_pte + 1));
            asm volatile("tlbwi");
            pte = ((page_paddr(free_index) >> 6) & 0xffffffc0) | PE_UC | PE_D | PE_V;
            *(p_pte + 1) = pte;
            if(old_pte & 0x80000000) {
                bcopy(swap_vaddr((old_pte & 0x7fffffff) >> 6), page_vaddr(free_index), PAGE_SIZE);
            } else {
                v_start = current_running->entry_point & 0xffffe000;
                l_start = current_running->loc - (current_running->entry_point - v_start);
                offset = ((badvpn2 << 13) | 0x1000) - v_start;
                if(offset < current_running->size + (current_running->entry_point - v_start))
                    bcopy(l_start + offset, page_vaddr(free_index), PAGE_SIZE);
            }
            print_str(19, 1, " ");
            printk("Invalid1 : %x, pte: %x", badvpn2, pte);
        }
        */
        // tlb_flush(entryhi);
        free_index = page_alloc(0, (uint32_t)(p_pte + entrylo_offset));
        old_pte = *(p_pte + entrylo_offset);
        pte = ((page_paddr(free_index) >> 6) & 0xffffffc0) | PE_UC | PE_D | PE_V;
        *(p_pte + entrylo_offset) = pte;
        if(entrylo_offset) set_cp0_entrylo1(*(p_pte + entrylo_offset));
        else set_cp0_entrylo0(*(p_pte + entrylo_offset));
        asm volatile("tlbwi");
        if(old_pte & 0x80000000) {
            bcopy(swap_vaddr((old_pte & 0x7fffffff) >> 6), page_vaddr(free_index), PAGE_SIZE);
        } else {
            v_start = current_running->entry_point & 0xffffe000;
            l_start = current_running->loc - (current_running->entry_point - v_start);
            offset = ((badvpn2 << 13) | (entrylo_offset << 12)) - v_start;
            if(offset < current_running->size + (current_running->entry_point - v_start))
                bcopy(l_start + offset, page_vaddr(free_index), PAGE_SIZE);
        }
        print_str(23 + tlb_refill_count + tlb_invalid_count, 1, " ");
        printk("Invalid%d : pid=%d, badvaddr=%x, old_pte=%x, pte=%x", entrylo_offset, entryhi & 0xff, badvaddr, old_pte, pte);
    } else {
        tlb_refill_count++;
        set_cp0_entrylo0(*(p_pte));
        set_cp0_entrylo1(*(p_pte + 1));
        asm volatile("tlbwr");
        print_str(23 + tlb_refill_count + tlb_invalid_count, 1, " ");
        printk("Refill : pid=%d, badvaddr=%x, lo0=%x, lo1=%x", entryhi & 0xff, badvaddr, *(p_pte), *(p_pte + 1));
        if(*(p_pte + entrylo_offset) & 0x2)
            set_R(*(p_pte + entrylo_offset));

    }

    print_str(20, 1, " ");
    printk("TLB Refill Count : %d", tlb_refill_count);
    print_str(21, 1, " ");
    printk("TLB Invalid Count: %d", tlb_invalid_count);

    // print_str(22, 1, " ");
    // printk("disable_count: %d", disable_count);
    return;
}
