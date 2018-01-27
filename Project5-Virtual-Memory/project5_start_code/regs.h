#define zero    $0
 
#define AT      $1
 
#define v0      $2
#define v1      $3
 
#define a0      $4
#define a1      $5
#define a2      $6
#define a3      $7
 
#define t0      $8
#define t1      $9
#define t2      $10
#define t3      $11
#define t4      $12
#define t5      $13
#define t6      $14
#define t7      $15
 
#define ta0     $12
#define ta1     $13
#define ta2     $14
#define ta3     $15
 
#define s0      $16
#define s1      $17
#define s2      $18
#define s3      $19
#define s4      $20
#define s5      $21
#define s6      $22
#define s7      $23
#define s8      $30             /* == fp */
 
#define t8      $24
#define t9      $25
#define k0      $26
#define k1      $27
 
#define gp      $28
 
#define sp      $29
#define fp      $30
#define ra      $31

/* This is for struct TrapFrame in scheduler.h
 * Stack layout for all exceptions:
 *
 * ptrace needs to have all regs on the stack. If the order here is changed,
 * it needs to be updated in include/asm-mips/ptrace.h
 *
 * The first PTRSIZE*5 bytes are argument save space for C subroutines.
 */

#define TF_REG0         0
#define TF_REG1         4
#define TF_REG2         8
#define TF_REG3         12
#define TF_REG4         16
#define TF_REG5         20
#define TF_REG6         24
#define TF_REG7         28
#define TF_REG8         32
#define TF_REG9         36
#define TF_REG10        40
#define TF_REG11        44
#define TF_REG12        48
#define TF_REG13        52
#define TF_REG14        56
#define TF_REG15        60
#define TF_REG16        64
#define TF_REG17        68
#define TF_REG18        72
#define TF_REG19        76
#define TF_REG20        80
#define TF_REG21        84
#define TF_REG22        88
#define TF_REG23        92
#define TF_REG24        96
#define TF_REG25        100
/*
 * $26 (k0) and $27 (k1) not saved
 */
#define TF_REG26        104
#define TF_REG27        108
#define TF_REG28        112
#define TF_REG29        116
#define TF_REG30        120
#define TF_REG31        124

#define TF_STATUS       128

#define TF_HI           132
#define TF_LO           136

#define TF_BADVADDR     140
#define TF_CAUSE        144
#define TF_EPC          148
#define TF_PC           152
/*
 * Size of stack frame, word/double word alignment
 */
#define TF_SIZE         156
