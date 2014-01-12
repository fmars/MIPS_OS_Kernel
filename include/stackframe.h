#include "regdef.h"
#include "cp0regdef.h"
#include "asm.h"
#include "env.h"

.macro STI
	mfc0	k0,	CP0_STATUS
	li		k1,  0x1
	or		k0, k1
	mtc0	k0, CP0_STATUS
	
.endm


.macro CLI
	mfc0	k0, CP0_STATUS
	ori		k0, k0, 0x1
	xori	k0, k0, 0x1
	mtc0	k0, CP0_STATUS
.endm

.macro SAVE_ALL    
                                  
		/*                                       
		 * Called from user mode, new stack      
		 */                                      
		//lui	k1,%hi(kernelsp)                 
		//lw	k1,%lo(kernelsp)(k1)  //not clear right now 
		           				
		move	k0,sp 
			
		li		sp, 0x90800000    // stackframe, save process enviroments here
		move	k1,sp                     
		subu	sp,k1,TF_SIZE 

		 
		sw		k0,TF_REG29(sp)   // ! save sp here
		                 
		mfc0	v0,CP0_STATUS                    
		sw		v0,TF_STATUS(sp)                 
		mfc0	v0,CP0_CAUSE                     
		sw		v0,TF_CAUSE(sp)                  
		mfc0	v0,CP0_EPC                       
		sw		v0,TF_EPC(sp)          
		mfhi	v0                               
		sw		v0,TF_HI(sp)                     
		mflo	v0                               
		sw	v0,TF_LO(sp)   
		
		sw	$0,TF_REG0(sp)
		sw	$1,TF_REG1(sp)                    
		sw  $2,TF_REG2(sp)         
		sw	$3,TF_REG3(sp)                   
		sw	$4,TF_REG4(sp)                   
		sw	$5,TF_REG5(sp)                   
		sw	$6,TF_REG6(sp)                   
		sw	$7,TF_REG7(sp)                   
		sw	$8,TF_REG8(sp)                   
		sw	$9,TF_REG9(sp)                   
		sw	$10,TF_REG10(sp)                 
		sw	$11,TF_REG11(sp)                 
		sw	$12,TF_REG12(sp)                 
		sw	$13,TF_REG13(sp)                 
		sw	$14,TF_REG14(sp)                 
		sw	$15,TF_REG15(sp)                 
		sw	$16,TF_REG16(sp)                 
		sw	$17,TF_REG17(sp)                 
		sw	$18,TF_REG18(sp)                 
		sw	$19,TF_REG19(sp)                 
		sw	$20,TF_REG20(sp)                 
		sw	$21,TF_REG21(sp)                 
		sw	$22,TF_REG22(sp)                 
		sw	$23,TF_REG23(sp)                 
		sw	$24,TF_REG24(sp)                 
		sw	$25,TF_REG25(sp)                 
		sw	$26,TF_REG26(sp)   		 
		sw	$27,TF_REG27(sp)   		 
		sw	$28,TF_REG28(sp)   
		//  $29,TF_REG29(sp)  mustn't save here, have already saved foreward
		sw	$30,TF_REG30(sp)                 
		sw	$31,TF_REG31(sp)
		
		move sp,k0 // 恢复sp值
		
		
		//subu  sp,16       // 这6条为打印寄存器信息，可注释掉
		//li  a0,0x90800000
		//subu	a0,a0,TF_SIZE
	//	addi  a1,$0,38
	//	jal   print_reg
	//	addu  sp,16 
		
.endm
/*
 * Note that we restore the IE flags from stack. This means
 * that a modified IE mask will be nullified.
 */
.macro RESTORE_SOME                                  
		mfc0	t0,CP0_STATUS                    
		ori	t0,0x3                          
		xori	t0,0x3                          
		mtc0	t0,CP0_STATUS                    
		lw	v0,TF_STATUS(sp)             
		li	v1, 0xff00 				 
		and	t0, v1 					 
		nor	v1, $0, v1 				 
		and	v0, v1 					 
		or	v0, t0 					 
		mtc0	v0,CP0_STATUS 		 
		lw	v1,TF_LO(sp)                                       
		//mtlo	v1                               
		lw	v0,TF_HI(sp)                     
		lw	v1,TF_EPC(sp)                    
		//mthi	v0                               
		mtc0	v1,CP0_EPC                       
		lw	$31,TF_REG31(sp)                 
		lw	$30,TF_REG30(sp)                 
		lw	$28,TF_REG28(sp)  
               
		lw	$25,TF_REG25(sp)                 
		lw	$24,TF_REG24(sp)                 
		lw	$23,TF_REG23(sp)                 
		lw	$22,TF_REG22(sp)                 
		lw	$21,TF_REG21(sp)                 
		lw	$20,TF_REG20(sp)                 
		lw	$19,TF_REG19(sp)                 
		lw	$18,TF_REG18(sp)                 
		lw	$17,TF_REG17(sp)                 
		lw	$16,TF_REG16(sp)                 
		lw	$15,TF_REG15(sp)                 
		lw	$14,TF_REG14(sp)                 
		lw	$13,TF_REG13(sp)                 
		lw	$12,TF_REG12(sp)                 
		lw	$11,TF_REG11(sp)                 
		lw	$10,TF_REG10(sp)                 
		lw	$9,TF_REG9(sp)                   
		lw	$8,TF_REG8(sp)                   
		lw	$7,TF_REG7(sp)                   
		lw	$6,TF_REG6(sp)                   
		lw	$5,TF_REG5(sp)                   
		lw	$4,TF_REG4(sp)                   
		lw	$3,TF_REG3(sp)                   
		lw	$2,TF_REG2(sp)                   
		lw	$1,TF_REG1(sp)     

		mfc0	k0,CP0_STATUS                    
		ori	    k0,k0,0x1                                                  
		mtc0	k0,CP0_STATUS  
	              
.endm
	
.macro RESTORE_ALL							 
		RESTORE_SOME								 
		lw	sp,TF_REG29(sp)  /* Deallocate stack */  
.endm

.set	noreorder
.macro RESTORE_ALL_AND_RET					 
		RESTORE_SOME							 				 
		lw	sp,TF_REG29(sp)  /* Deallocate stack */  
		eret						 
.endm


