#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>

LEAF(msyscall)

/////////////////////////////////////////
//insert your code here
//put the paramemters to the right pos
//and then call the "syscall" instruction
/////////////////////////////////////////
lw v0,0(sp)
lw a0,4(sp)
lw a1,8(sp)
lw a2,12(sp)
lw a3,16(sp)
syscall

jr	ra

END(msyscall)
