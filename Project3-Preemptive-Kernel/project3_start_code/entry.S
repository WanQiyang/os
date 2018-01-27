#include "asm.h"
#include "regs.h"
#include "cp0regdefs.h"

  /* Hardcoded values. Do not change these!! */
  /* Offset of the kernel context in a PCB */
  .equ  KERNEL, 8
  /* Offset of the user context in a PCB */
  .equ  USER, 164
  /* Offset of %esp in a context */
  .equ  STACK, TF_REG29
  /* Offset of field nested_count in a PCB */
  .equ  NESTED_COUNT, 320

.macro PUTCHAR ch label
	  addi  k0,$0,10000
	\label:
	addi  k0,k0,-1
	bne k0,$0,\label

	addi  k0,$0,\ch
	la    k1,0xbfe48000
	sb    k0,0(k1)
.endm

.macro STI
  mfc0    k0, CP0_STATUS
  li      k1, (STATUS_CU0 | 0x1)
  or      k0, k1
  mtc0    k0, CP0_STATUS
.endm

.macro CLI
  mfc0    k0, CP0_STATUS
  li      k1, (STATUS_CU0 | 0x1)
  or      k0, k1
  xori    k0, 0x1
  mtc0    k0, CP0_STATUS
.endm

/* Do not change any of these macros! */
/* Save registers/flags to the specified offset in the current PCB */
.macro SAVE_CONTEXT offset
/* TODO: need add */
  lw      k0, current_running
  addi    k0, k0, \offset

  sw      AT, 4(k0)
  sw      v0, 8(k0)
  sw      v1, 12(k0)
  sw      a0, 16(k0)
  sw      a1, 20(k0)
  sw      a2, 24(k0)
  sw      a3, 28(k0)
  sw      t0, 32(k0)
  sw      t1, 36(k0)
  sw      t2, 40(k0)
  sw      t3, 44(k0)
  sw      t4, 48(k0)
  sw      t5, 52(k0)
  sw      t6, 56(k0)
  sw      t7, 60(k0)
  sw      s0, 64(k0)
  sw      s1, 68(k0)
  sw      s2, 72(k0)
  sw      s3, 76(k0)
  sw      s4, 80(k0)
  sw      s5, 84(k0)
  sw      s6, 88(k0)
  sw      s7, 92(k0)
  sw      t8, 96(k0)
  sw      t9, 100(k0)
  /* sw      k0, 104(k0) */
  /* sw      k1, 108(k0) */
  sw      gp, 112(k0)
  sw      sp, 116(k0)
  sw      fp, 120(k0)
  sw      ra, 124(k0)

  mfc0    k1, CP0_STATUS
  nop
  sw      k1, 128(k0)
  mfc0    k1, CP0_TAGHI
  nop
  sw      k1, 132(k0)
  mfc0    k1, CP0_TAGLO
  nop
  sw      k1, 136(k0)
  mfc0    k1, CP0_BADVADDR
  nop
  sw      k1, 140(k0)
  mfc0    k1, CP0_CAUSE
  nop
  sw      k1, 144(k0)
  mfc0    k1, CP0_EPC
  nop
  sw      k1, 148(k0)
/* TODO: end */
.endm

/* Restore registers/flags from the specified offset in the current PCB */
.macro RESTORE_CONTEXT offset
/* TODO: need add */
  lw      k0, current_running
  addi    k0, k0, \offset

  lw      AT, 4(k0)
  lw      v0, 8(k0)
  lw      v1, 12(k0)
  lw      a0, 16(k0)
  lw      a1, 20(k0)
  lw      a2, 24(k0)
  lw      a3, 28(k0)
  lw      t0, 32(k0)
  lw      t1, 36(k0)
  lw      t2, 40(k0)
  lw      t3, 44(k0)
  lw      t4, 48(k0)
  lw      t5, 52(k0)
  lw      t6, 56(k0)
  lw      t7, 60(k0)
  lw      s0, 64(k0)
  lw      s1, 68(k0)
  lw      s2, 72(k0)
  lw      s3, 76(k0)
  lw      s4, 80(k0)
  lw      s5, 84(k0)
  lw      s6, 88(k0)
  lw      s7, 92(k0)
  lw      t8, 96(k0)
  lw      t9, 100(k0)
  /* lw      k0, 104(k0) */
  /* lw      k1, 108(k0) */
  lw      gp, 112(k0)
  lw      sp, 116(k0)
  lw      fp, 120(k0)
  lw      ra, 124(k0)

  lw      k1, 128(k0)
  mtc0    k1, CP0_STATUS
  nop
  lw      k1, 132(k0)
  mtc0    k1, CP0_TAGHI
  nop
  lw      k1, 136(k0)
  mtc0    k1, CP0_TAGLO
  nop
  lw      k1, 140(k0)
  mtc0    k1, CP0_BADVADDR
  nop
  lw      k1, 144(k0)
  mtc0    k1, CP0_CAUSE
  nop
  lw      k1, 148(k0)
  mtc0    k1, CP0_EPC
  nop
/* TODO: end */
.endm

/* Increment disable_count, panicking on a double ENTER
 * Interrupts should be disabled before this macro is used
 */
.macro ENTER_CRITICAL
  la      k0, disable_count
  lw      k1, 0(k0)
  bne     k1, $0, panice
  nop
  addi    k1, k1, 1
  sw      k1, 0(k0)
.endm

/* Decrement disable_count, panicking on a double LEAVE
 * Interrupts should be enabled after this macro is used
 */
.macro LEAVE_CRITICAL
  la      k0, disable_count
  lw      k1, 0(k0)
  addi    k1, k1, -1
  bne     k1, $0, panicl
  nop
  sw      k1, 0(k0)
.endm

/* TODO: Test current_running->nested_count, clobbering only eflags */
.macro TEST_NESTED_COUNT
  lw      k1, current_running
  lw      k1, NESTED_COUNT(k1)
.endm

/** TODO end */

  .data
// Temporary storage for 32-bit registers
scratch:
  .long    0
// Temporary storage for 32-bit registers
scratch2:
  .long    0
// Temporary storage for syscall return value
return_value:
  .long    0
// Signals status of interrupts
    .globl    disable_count
disable_count:
  .long    0

  .text

NESTED(scheduler_entry,0,ra)
	/* TODO: need add */
  SAVE_CONTEXT(KERNEL)
  jal     scheduler
  nop
  RESTORE_CONTEXT(KERNEL)
  jr      ra
  nop
	/* TODO: end */
END(scheduler_entry)

LEAF(asm_start)
  /* Disable interrupts */
  mtc0    zero, CP0_STATUS

  /* Disable watch exception. */
  mtc0    zero, CP0_WATCHLO
  mtc0    zero, CP0_WATCHHI

  /* disable kernel mode cache */
  mfc0    t0, CP0_CONFIG
  and     t0, ~0x7
  ori     t0, 0x2
  mtc0    t0, CP0_CONFIG

  jr  ra
END(asm_start)

LEAF(reset_timer)
  mtc0  $0, CP0_COUNT
  mtc0  a0, CP0_COMPARE
  jr    ra
END(reset_timer)

  .global exception_handler_start
  .global exception_handler_end
NESTED(general_exception_handler,0,sp)
exception_handler_start:
  .set  noat
  .set  mips32

  CLI

  mfc0  k0, CP0_CAUSE
  andi  k0, k0, CAUSE_EXCCODE   /* k0 = {exc_code,00} */
  la    k1, interrupt_handlers
  add   k0, k0, k1
  lw    k0, 0(k0)
  j     k0     /* interrupt_handlers[exc_code](); */
exception_handler_end:
END(general_exception_handler)
  .set at
  .set reorder
  .text

LEAF(return_from_exception)
	STI
	eret
END(return_from_exception)

NESTED(simple_handler,0,sp)
  /* SAVE_CONTEXT(KERNEL) */
  SAVE_CONTEXT(USER)

  mfc0  k0, CP0_CAUSE           /* Read Cause register for IP bits */
  //mfc0  k1, CP0_STATUS        /* and Status register for IM bits */
  andi  k0, k0, CAUSE_IPL       /* Keep only IP bits from Cause */
  //and   k0, k0, k1            /* and mask with IM bits */
  clz   k0, k0                  /* Find first bit set, IP7..IP0; k0 = 16..23 */
  xori  k0, k0, 0x17            /* 16..23 => 7..0 */
  add   a0, k0, $0

  mfc0  k0, CP0_CAUSE
	andi  k0, k0, CAUSE_EXCCODE   /* k0 = {exc_code,00} */
  srl   k0, k0, 2
  add   a1, k0, $0

  jal   c_simple_handler

  mfc0  k0, CP0_CAUSE           /* Read Cause register for IP bits */
  nop
	andi  k1, k0, CAUSE_IPL       /* Keep only IP bits from Cause */
	xor   k0, k0, k1
  mtc0  k0, CP0_CAUSE
  nop

  LEAVE_CRITICAL
  /* RESTORE_CONTEXT(KERNEL) */
  RESTORE_CONTEXT(USER)
  j     return_from_exception
  nop
END(simple_handler)

NESTED(handle_syscall,0,sp)
  SAVE_CONTEXT(USER)
  // RESTORE_CONTEXT(KERNEL)
  add   a3,a2,$0
  add   a2,a1,$0
  add   a1,a0,$0
  add   a0,v0,$0

  jal   system_call_helper

  LEAVE_CRITICAL
  RESTORE_CONTEXT(USER)
  j     return_from_exception

END(handle_syscall)

NESTED(handle_int,0,sp)
  /* TODO: timer_irq */
  /* read int IP and handle clock interrupt or just call do_nothing */
  SAVE_CONTEXT(USER)

  mfc0  k0, CP0_CAUSE
  nop
  andi  k0, k0, CAUSE_IPL
  clz   k0, k0
  xori  k0, k0, 0x17

  addiu k1, zero, 7
  beq   k0, k1, time_irq
  nop

  jal   clear_int

int_finish:
  RESTORE_CONTEXT(USER)
  j     return_from_exception
  nop

#define TIMER_INTERVAL 150000000

time_irq:
  li    a0, TIMER_INTERVAL
  jal   reset_timer

  jal   clear_int

  lw    k1, time_elapsed
  addiu k1, k1, 1000
  sw    k1, time_elapsed
  TEST_NESTED_COUNT
  bne   k1, zero, int_finish
  nop

  ENTER_CRITICAL
  lw    k0, current_running
  li    k1, 1
  sw    k1, NESTED_COUNT(k0)

  jal   put_current_running
  nop
  jal   scheduler_entry
  nop

  lw    k0, current_running
  sw    zero, NESTED_COUNT(k0)

  LEAVE_CRITICAL
  j     int_finish
  nop

clear_int:
  mfc0  k0, CP0_CAUSE
  nop
  andi  k1, k0, CAUSE_IPL
  xor   k0, k0, k1
  mtc0  k0, CP0_CAUSE
  nop
  jr    ra

  /* TODO:end */
END(handle_int)

LEAF(first_entry)
  LEAVE_CRITICAL
  j     int_finish
END(first_entry)

LEAF(get_cp0_status)
  mfc0  v0, CP0_STATUS
  jr    ra
END(get_cp0_status)

LEAF(set_cp0_status)
  mtc0  a0, CP0_STATUS
  jr    ra
END(set_cp0_status)


LEAF(enter_critical)
  CLI
  ENTER_CRITICAL
  jr    ra
END(enter_critical)

LEAF(leave_critical)
  LEAVE_CRITICAL
  STI
  jr    ra
END(leave_critical)

/* Error Message: Write "PANIC" to the screen and enter an infinite loop */
LEAF(panic)
  CLI
  PUTCHAR 0x50 P1// 'P'
  j     panic
END(panic)

LEAF(panice)
  CLI
  PUTCHAR 69 P2// 'P'
  j     panic
END(panice)

LEAF(panicl)
  CLI
  PUTCHAR 76 P3// 'P'
  j     panic
END(panicl)

LEAF(infinite_loop)
  j    infinite_loop
END(infinite_loop)
