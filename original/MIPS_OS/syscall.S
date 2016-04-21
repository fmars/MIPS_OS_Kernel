#include "include/regdef.h"
#include "include/cp0regdef.h"
#include "include/asm.h"
#include "include/stackframe.h"
#include "include/unistd.h"


// handle_sys 系统调用处理函数
NESTED(handle_sys,TF_SIZE, sp)
CLI
SAVE_ALL
nop


.set at
lw 	t1, TF_EPC(sp)
lw 	v0, TF_REG2(sp)

subu 	v0, v0, __SYSCALL_BASE
sltiu 	t0, v0, __NR_SYSCALLS+1

addiu   t1, 4
sw	t1, TF_EPC(sp)
beqz	t0,  illegal_syscall
nop
sll	t0, v0,2
la	t1, sys_call_table
addu	t1, t0
lw	t2, (t1)
beqz	t2, illegal_syscall 
nop

lw	t0,TF_REG29(sp)

lw	t1, TF_REG4(sp)
lw	t3, TF_REG5(sp) 
lw	t4, TF_REG6(sp)
lw	t5, TF_REG7(sp)
lw	t6, TF_REG8(sp)
lw	t7, TF_REG9(sp)

subu	sp, 20

sw	t1, 0(sp)
sw	t3, 4(sp)
sw	t4, 8(sp)
sw	t5, 12(sp)
sw	t6, 16(sp)
sw	t7, 20(sp)

move	a0, t1
move	a1, t3
move	a2, t4
move	a3, t5
	
jr	t2 

nop
addu	sp, 20

sw	v0, TF_REG2(sp)

j	ret_from_exception 

nop

illegal_syscall: j illegal_syscall
nop
END(handle_sys)


LEAF(pds_syscall) // 所用系统调用的syscall入口函数
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
	
	//jr		ra
	j	ret_from_exception
	
	nop
END(pds_syscall)





.extern syscall_putchar
.extern syscall_printf
.extern syscall_exit
	
.macro syscalltable  // 系统调用表
.word syscall_putchar
.word syscall_printf
.word syscall_exit
.endm


EXPORT(sys_call_table)
syscalltable
.size sys_call_table, . - sys_call_table 




FEXPORT(ret_from_exception)
	.set noat  
	.set noreorder
	RESTORE_SOME
	
	// 异常返回时，EPC加4 
    mfc0    $26, $14                // get original PC where exception occuried
    addiu   $26, $26, 4             // point to the instruction to resume
    mtc0    $26, $14                // write to EPC
	.set at			 
	lw	sp,TF_REG29(sp) /* Deallocate stack */  
	nop

	eret   
	nop



