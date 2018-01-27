#include "asm.h"

LEAF(invoke_syscall)
  sw    a1,0(sp)
  sw    a2,4(sp)
  sw    a3,8(sp)
  add   v0, a0, $0
  add   a0, a1, $0
  add   a1, a2, $0
  add   a2, a3, $0

  syscall

  jr      ra
END(invoke_syscall)
