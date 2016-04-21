#include "include/asm.h"
#include "include/regdef.h"
#include "include/cp0regdef.h"
#include "include/stackframe.h"

// 异常入口0XBFC00380（BEV=1）， 只可写8条指令	
.section .text.exc_entry
.set noreorder
.global exc_entry
exc_entry:
/* step1 : judge exception type */
    
/* step2 : select correct stack */

/* step3 : saving context to stack */

/* step4 : branch to USER handle */

/* step5 : restore context from stack */

/* step6 : return */

	CLI	
	j     handle_sys  // 现在只处理系统调用的异常
			 
	 
	 

// 中断入口0XBFC00400（BEV=1） 
.section .text.int_entry
.set noreorder
.set noreorder
.global int_entry
int_entry:
	//CLI
	mfc0	k1,CP0_CAUSE
	la	k0,exception_handlers

	andi	k1,0x7c // 0111_1100 get ExcCode
	addu	k0,k1
	lw	k0,(k0)

	NOP
	j k0
	nop			 

						 					
						 
						 
/****************************************************************************/
/***********************  handle_int  ***************************************/
// 中断处理函数，出口为env_pop_tf的 eret 指令， 回到要运行的用户进程
// 注意：中断处理，EPC不用加4

.text
.set noreorder
NESTED(handle_int, TF_SIZE, sp)
.set  noat            
nop

//jal signal_int

SAVE_ALL  //    保存寄存器值

jal signal_int
j   timer_irq  // 现在只处理时钟中断引起的 进程调度

nop
END(handle_int)



	.extern sched_yield

timer_irq:

1:	j	sched_yield  // 跳到进程调度函数
	nop








