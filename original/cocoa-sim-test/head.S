/* CP0 regs */
#include "mipsregs.h"
/* CPU regs */
#include "regdef.h"
.text
.global _start
_start:
    li  s0, 0xa0000000
    subu s0, 16
    move sp, s0
    b   main
    nop
    nop
.global uput
uput:
    lui t0, 0xa000
    sw v0, 0x700(t0)
    jr ra

