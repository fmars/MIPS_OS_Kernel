#include "include/regdef.h"
#include "include/asm.h"
#include "include/env.h"
#include "include/stackframe.h"

		.data
		.global	KERNEL_SP;
KERNEL_SP:
		.word		0

		
		.text
		.set noreorder
	//	.set noat
LEAF(env_pop_tf)   //  恢复将要运行程序的 寄存器值
		add		k0,a0,0                   		
	
		subu  sp,16   // 这6条为打印寄存器信息，可注释掉
		move  a0,k0
		addi  a1,$0,38
		jal   print_reg
		addu  sp,16	
		jal     leave_int	
		
		
		lw	v1,TF_LO(k0)                                       
		//mtlo	v1                               
		lw	v0,TF_HI(k0)
	    //mthi	v0  
		
		lw		v1,TF_EPC(k0)                    	                             
		//add		v1,v1,0x4
		mtc0	v1,CP0_EPC   // epc must recover         
	
		
		lw	$31,TF_REG31(k0)                 
		lw	$30,TF_REG30(k0)     
	    lw	$29,TF_REG29(k0)  // sp must recover 
		lw	$28,TF_REG28(k0)  
		//	$27,TF_REG27(k0)  // k1 not recover               
		//	$26,TF_REG26(k0)  // k0 not recover	
        //sw	$26,TF_REG26(k0)   
		lw	$25,TF_REG25(k0)                 
		lw	$24,TF_REG24(k0)                 
		lw	$23,TF_REG23(k0)                 
		lw	$22,TF_REG22(k0)                 
		lw	$21,TF_REG21(k0)                 
		lw	$20,TF_REG20(k0)                 
		lw	$19,TF_REG19(k0)                 
		lw	$18,TF_REG18(k0)                 
		lw	$17,TF_REG17(k0)                 
		lw	$16,TF_REG16(k0)                 
		lw	$15,TF_REG15(k0)                 
		lw	$14,TF_REG14(k0)                 
		lw	$13,TF_REG13(k0)                 
		lw	$12,TF_REG12(k0)                 
		lw	$11,TF_REG11(k0)                 
		lw	$10,TF_REG10(k0)                 
		lw	$9,TF_REG9(k0)                   
		lw	$8,TF_REG8(k0)                   
		lw	$7,TF_REG7(k0)                   
		lw	$6,TF_REG6(k0)                   
		lw	$5,TF_REG5(k0)                   
		lw	$4,TF_REG4(k0)                   
		lw	$3,TF_REG3(k0)                   
		lw	$2,TF_REG2(k0)                   
		lw	$1,TF_REG1(k0)
		lw	$0,TF_REG0(k0)

		lui     k1,0xa000  // timer0 模式0，重新启动计数
		lui		v1,0x17d
		ori		v1,v1,0x7840
		sw		v1,0x204(k1)
		li		v1,0x1
		sw		v1,0x200(k1)
 
		mfc0	k1,CP0_STATUS
		ori		k1,k1,0x1         // must enable interrupt IE=1
		mtc0	k1,CP0_STATUS
		
		eret
		
		nop
END(env_pop_tf)


