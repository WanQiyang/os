#include "regs.h"
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
	.align	2
	.globl  kernel_entry
	.ent	kernel_entry
	.type	kernel_entry, @function
kernel_entry:
	addiu sp, sp, -24
	sw ra, 0(sp)
	bnez $4, 1f
	nop

	jal do_yield
	nop
	beqz $0,2f
	nop

1:
	jal do_exit
	nop
2:
	lw ra, 0(sp)
	addiu sp, sp, 24
	jr ra
	nop

	.align	2
	.globl  scheduler_entry
	.ent	scheduler_entry
	.type	scheduler_entry, @function
scheduler_entry:
	# call scheduler, which will set new current process
	# need student add
  jal scheduler
  lw $31, current_running
  lw $1, 28($31)
  lw $2, 32($31)
  lw $3, 36($31)
  lw $4, 40($31)
  lw $5, 44($31)
  lw $6, 48($31)
  lw $7, 52($31)
  lw $8, 56($31)
  lw $9, 60($31)
  lw $10, 64($31)
  lw $11, 68($31)
  lw $12, 72($31)
  lw $13, 76($31)
  lw $14, 80($31)
  lw $15, 84($31)
  lw $16, 88($31)
  lw $17, 92($31)
  lw $18, 96($31)
  lw $19, 100($31)
  lw $20, 104($31)
  lw $21, 108($31)
  lw $22, 112($31)
  lw $23, 116($31)
  lw $24, 120($31)
  lw $25, 124($31)
  # lw $26, 128($31)
  # lw $27, 132($31)
  lw $28, 136($31)
  lw $29, 140($31)
  lw $30, 144($31)
  lw $31, 148($31)
  # break 111
	jr ra
  nop

	.align	2
	.globl  save_pcb
	.ent	save_pcb
	.type	save_pcb, @function
save_pcb:
	# save the pcb of the currently running process
	# need student add
  lw $27, 28(sp)
  addiu sp, sp, 32
  lw $26, current_running
  sw $1, 28($26)
  sw $2, 32($26)
  sw $3, 36($26)
  sw $4, 40($26)
  sw $5, 44($26)
  sw $6, 48($26)
  sw $7, 52($26)
  sw $8, 56($26)
  sw $9, 60($26)
  sw $10, 64($26)
  sw $11, 68($26)
  sw $12, 72($26)
  sw $13, 76($26)
  sw $14, 80($26)
  sw $15, 84($26)
  sw $16, 88($26)
  sw $17, 92($26)
  sw $18, 96($26)
  sw $19, 100($26)
  sw $20, 104($26)
  sw $21, 108($26)
  sw $22, 112($26)
  sw $23, 116($26)
  sw $24, 120($26)
  sw $25, 124($26)
  # sw $26, 128($26)
  # sw $27, 132($26)
  sw $28, 136($26)
  sw $29, 140($26)
  sw $30, 144($26)
  sw $27, 148($26)
  addiu sp, sp, -32
  jr ra
  nop
