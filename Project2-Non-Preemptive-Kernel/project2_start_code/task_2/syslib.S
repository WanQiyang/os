#include "regs.h"

.text
/* address of kernel_entry function may be modified, zxj */
#define SYSCALL(i) \
	li a0,i; \
	li $8,0xa0800520; \
	jal $8; \
	nop
	nop

.globl yield
.globl exit

yield :
	addiu sp,sp,-24
	sw ra,0(sp)
	sw a0,8(sp)
	sw a1,16(sp)
	SYSCALL(0)
	lw a1,16(sp)
	lw a0,8(sp)
	lw ra,0(sp)
	addiu sp,sp,24
	jr ra
	nop


	exit :
	addiu sp,sp,-24
	sw ra,0(sp)
	sw a0,8(sp)
	sw a1,16(sp)
	SYSCALL(1)
	lw a1,16(sp)
	lw a0,8(sp)
	lw ra,0(sp)
	addiu sp,sp,24
	jr ra
	nop

	.text
	.align	2
	.globl  get_timer
	.ent	get_timer
	.type	get_timer, @function
	get_timer:
	mfc0 $2,$9
	nop
	j       ra
	nop

	.data
ENTRY_POINT: .word 0xa080fff8
