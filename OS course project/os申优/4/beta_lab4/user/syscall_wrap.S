#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>

LEAF(msyscall)

/////////////////////////////////////////
//insert your code here
//put the paramemters to the right pos
//and then call the "syscall" instruction
/////////////////////////////////////////
//fmars:
//j fmars
move v0,a0
sw	a0,0(sp)
sw	a1,4(sp)
sw	a2,8(sp)
sw	a3,12(sp)
syscall

jr	ra

END(msyscall)

//move a0->v0
//move a1-3 to stack
