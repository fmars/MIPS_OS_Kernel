#include "include/asm.h"
#include "include/regdef.h"
#include "include/cp0regdef.h"

.section .text.entry
.set noreorder

// -------------Program Start Place---0xBFC00000---------------- 
LEAF(_start_)  
	li     	k0, 0x90700000 // 我设定的系统栈指针sp=0x90700000
	subu    k0,512
	move 	sp,k0

	j	main
	nop
END(_start_)


LEAF(start_timer)  
	lui     k1,0xa000  // timer0 模式0
	lui		v1,0x0
	ori		v1,v1,0x271
	sw		v1,0x204(k1) // timer0 初值寄存器
	li		v1,0x1
	sw		v1,0x200(k1)   // timer0 控制寄存器


	 mfc0 	k0, $12      // cp0
	 ori  	k0,k0,0x2001
	 mtc0 	k0, $12  
	
	 jr 	ra
	nop
END(start_timer)

//fmars
LEAF( uput )
    lui t0, 0xa000
    sw v0, 0x700(t0)
    jr ra
    nop
END( uput)




