/* See COPYRIGHT for copyright information. */

#ifndef _ENV_H_
#define _ENV_H_

#ifndef __ASSEMBLER__

#include "mm.h" 
#include "queue.h"

#define TIMESTACK 0x90800000

#define LOG2NENV	6
#define NENV		(1<<LOG2NENV)  // 64
#define ENVX(envid)	((envid) & (NENV - 1))

#define ENV_FREE	0
#define ENV_RUNNABLE		1
#define ENV_NOT_RUNNABLE	2


struct Trapframe { //lr:need to be modified(reference to linux pt_regs) TODO
	/* Saved main processor registers. */
	unsigned long regs[32];

	/* Saved special registers. */
	unsigned long cp0_status;
	unsigned long hi;
	unsigned long lo;
	unsigned long cp0_cause;
	unsigned long cp0_epc;
	unsigned long pc;
};

void *set_except_vector(int n, void * addr);

void trap_init();



struct Env {
	struct Trapframe env_tf;        // Saved registers
	u_int env_id;                   // Unique environment identifier
	u_int env_status;               // Status of the environment
	LIST_ENTRY(Env) env_link;       // Free list	
};

extern struct Env envs[NENV];		// All environments
extern struct Env *curenv;	        // the current env


void env_init(void);
void env_destroy(struct Env *);
void env_create(u_char *binary,u_int size);
void env_run(struct Env *e);


#define ENV_CREATE(x) \
{ \
	extern u_char binary_user_##x##_start[];\
	extern u_int binary_user_##x##_size; \
	env_create(binary_user_##x##_start, \
		(u_int)binary_user_##x##_size); \
}

#define ID2UTOP(id) ((id)*0x100000+0x80000000);

#endif /* !__ASSEMBLER__ */


/*
 * Stack layout for all exceptions:
 *
 * ptrace needs to have all regs on the stack. If the order here is changed,
 * it needs to be updated in include/asm-mips/ptrace.h
 *
 * The first PTRSIZE*5 bytes are argument save space for C subroutines.
 */

#define TF_REG0		0
#define TF_REG1		((TF_REG0) + 4)
#define TF_REG2		((TF_REG1) + 4)
#define TF_REG3		((TF_REG2) + 4)
#define TF_REG4		((TF_REG3) + 4)
#define TF_REG5		((TF_REG4) + 4)
#define TF_REG6		((TF_REG5) + 4)
#define TF_REG7		((TF_REG6) + 4)
#define TF_REG8		((TF_REG7) + 4)
#define TF_REG9		((TF_REG8) + 4)
#define TF_REG10	((TF_REG9) + 4)
#define TF_REG11	((TF_REG10) + 4)
#define TF_REG12	((TF_REG11) + 4)
#define TF_REG13	((TF_REG12) + 4)
#define TF_REG14	((TF_REG13) + 4)
#define TF_REG15	((TF_REG14) + 4)
#define TF_REG16	((TF_REG15) + 4)
#define TF_REG17	((TF_REG16) + 4)
#define TF_REG18	((TF_REG17) + 4)
#define TF_REG19	((TF_REG18) + 4)
#define TF_REG20	((TF_REG19) + 4)
#define TF_REG21	((TF_REG20) + 4)
#define TF_REG22	((TF_REG21) + 4)
#define TF_REG23	((TF_REG22) + 4)
#define TF_REG24	((TF_REG23) + 4)
#define TF_REG25	((TF_REG24) + 4)
/*
 * $26 (k0) and $27 (k1) not saved
 */
#define TF_REG26	((TF_REG25) + 4)
#define TF_REG27	((TF_REG26) + 4)
#define TF_REG28	((TF_REG27) + 4)
#define TF_REG29	((TF_REG28) + 4)
#define TF_REG30	((TF_REG29) + 4)
#define TF_REG31	((TF_REG30) + 4)

#define TF_STATUS	((TF_REG31) + 4)

#define TF_HI		((TF_STATUS) + 4)
#define TF_LO		((TF_HI) + 4)

#define TF_CAUSE	((TF_LO) + 4)
#define TF_EPC		((TF_CAUSE) + 4)
#define TF_PC		((TF_EPC) + 4)
/*
 * Size of stack frame, word/double word alignment
 */
#define TF_SIZE		((TF_PC)+4)

// Values of env_status in struct Env


#endif // !_ENV_H_
