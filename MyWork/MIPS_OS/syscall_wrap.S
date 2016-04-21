#include "include/asm.h"
#include "include/regdef.h"
#include "include/cp0regdef.h"


LEAF(msyscall)
	move	v0, a0
	subu  sp,32
	sw		ra, 28(sp)
	sw		a0, 0(sp)		
	sw		a1, 4(sp)
	sw		a2, 8(sp)
	sw		a3, 12(sp)

	syscall

	lw		ra,28(sp)
	addu		sp,32
	jr		ra
	nop
END(msyscall)
