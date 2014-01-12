#include "include/regdef.h"
#include "include/cp0regdef.h"
#include "include/asm.h"
#include "include/stackframe.h"

.macro	__build_clear_sti
	STI
.endm

.macro	__build_clear_cli
	CLI
.endm

.macro	setup_c0_status set clr
	.set	push
	mfc0	t0, CP0_STATUS
	or	t0, \set|\clr
	xor	t0, \clr
	mtc0	t0, CP0_STATUS			
	.set	pop
.endm

.macro	BUILD_HANDLER exception handler clear
	.align	5
	NESTED(handle_\exception, TF_SIZE, sp)  
	//.set	noat

	nop

	SAVE_ALL				
	__build_clear_\clear
	.set	at
	move	a0, sp
	jal	\handler
	nop
	j	ret_from_exception
	nop
	END(handle_\exception)
.endm



LEAF(MOD)
	divu	zero,a0,a1
	bnez	a1,1f
	nop
	break	0x7
1:
	mfhi	v0
	jr	ra
	nop
END(MOD)

LEAF(DIV)
	divu	zero,a0,a1
	bnez	a1,1f
	nop
	break	0x7
1:
	mflo	v0
	jr	ra
	nop
END(DIV)




LEAF(do_reserved)
END(do_reserved)


BUILD_HANDLER reserved do_reserved cli





