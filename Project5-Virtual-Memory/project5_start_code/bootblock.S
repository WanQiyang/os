
.text
.globl main

main:
    # check the offset of main
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

real_start:
    li $4, 0xa0800200
    li $5, 0x200
    lw $6, main
    sll $6, $6, 9
    jal 0x8007b1a8
    nop
    jal 0xa0800200

real_end:
    j real_end
