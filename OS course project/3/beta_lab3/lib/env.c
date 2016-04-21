/* See COPYRIGHT for copyright information. */

#include <mmu.h>
#include <error.h>
#include <env.h>
#include <pmap.h>
#include <printf.h>

struct Env *envs = NULL;		// All environments
struct Env *curenv = NULL;	        // the current env

static struct Env_list env_free_list;	// Free list

extern Pde * boot_pgdir;
//static int KERNEL_SP;
extern char * KERNEL_SP;
//
// Calculates the envid for env e.  
//
u_int mkenvid(struct Env *e)
{
	static u_long next_env_id = 0;
	// lower bits of envid hold e's position in the envs array
	u_int idx = e - envs;
	
	//printf("env.c:mkenvid:\txx:%x\n",(int)&idx);

	// high bits of envid hold an increasing number
	return(++next_env_id << (1 + LOG2NENV)) | idx;
}

//
// Converts an envid to an env pointer.
//
// RETURNS
//   env pointer -- on success and sets *error = 0
//   NULL -- on failure, and sets *error = the error number
//
int envid2env(u_int envid, struct Env **penv, int checkperm)
{
  
	struct Env *e;
	u_int cur_envid;
	if (envid == 0) {
		*penv = curenv;
		return 0;
	}

	e = &envs[ENVX(envid)];
	if (e->env_status == ENV_FREE || e->env_id != envid) {
		*penv = 0;
		return -E_BAD_ENV;
	}

	if (checkperm) {
		cur_envid = envid;
		while (&envs[ENVX(cur_envid)] != curenv && ENVX(cur_envid) != 0)
		{
			envid = envs[ENVX(cur_envid)].env_parent_id;
			cur_envid = envid;
		}
		if (ENVX(cur_envid) == 0)
		{
			*penv = 0;
			return -E_BAD_ENV;
		}
	}
	*penv = e;
	return 0;
	
}


//
// Marks all environments in 'envs' as free and inserts them into 
// the env_free_list.  Insert in reverse order, so that
// the first call to env_alloc() returns envs[0].
//
void
env_init(void)
{
	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

	//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
}

//
// Initializes the kernel virtual memory layout for environment e.
//
// Allocates a page directory and initializes it.  Sets
// e->env_cr3 and e->env_pgdir accordingly.
//
// RETURNS
//   0 -- on sucess
//   <0 -- otherwise 
//
static int
env_setup_vm(struct Env *e)
{
	// Hint:

	int i, r;
	struct Page *p = NULL;

	Pde *pgdir;
	// Allocate a page for the page directory
	if ((r = page_alloc(&p)) < 0)
	{
		panic("env_setup_vm - page_alloc error\n");
			return r;
	}
	p->pp_ref++;

	// Hint:
	//    - The VA space of all envs is identical above UTOP
	//      (except at VPT and UVPT) 
	//    - Use boot_pgdir
	//    - Do not make any calls to page_alloc 
	//    - Note: pp_refcnt is not maintained for physical pages mapped above UTOP.

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

	//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	return 0;
}

//
// Allocates and initializes a new env.
//
// RETURNS
//   0 -- on success, sets *new to point at the new env 
//   <0 -- on failure
//
int
env_alloc(struct Env **new, u_int parent_id)
{
	int r;
	struct Env *e;

	if (!(e = LIST_FIRST(&env_free_list)))
		return -E_NO_FREE_ENV;
	if ((r = env_setup_vm(e)) < 0)
		return r;
	e->env_id = mkenvid(e);
	e->env_parent_id = parent_id;
	e->env_status = ENV_RUNNABLE;

	// Set initial values of registers
	//  (lower 2 bits of the seg regs is the RPL -- 3 means user process)
	e->env_tf.regs[29] = USTACKTOP;
	e->env_tf.cp0_status = 0x10001004;
	// You also need to set tf_eip to the correct value.
	// Hint: see load_icode
/*
	e->env_ipc_blocked = 0;
	e->env_ipc_value = 0;
	e->env_ipc_from = 0;
*/
	e->env_ipc_recving = 0;
	e->env_pgfault_handler = 0;
	e->env_xstacktop = 0;
//printf("come 2\n");
	// commit the allocation
	LIST_REMOVE(e, env_link);
	*new = e;
	return 0;
}

//
// Sets up the the initial stack and program binary for a user process.
//
// This function loads the complete binary image, including a.out header,
// into the environment's user memory starting at virtual address UTEXT,
// and maps one page for the program's initial stack
// at virtual address USTACKTOP - BY2PG.
// Since the a.out header from the binary is mapped at virtual address UTEXT,
// the actual program text starts at virtual address UTEXT+0x20.
//
// This function does not allocate or clear the bss of the loaded program,
// and all mappings are read/write including those of the text segment.
//
static void
load_icode(struct Env *e, u_char *binary, u_int size)
{
	// Hint: 
	//  Use page_alloc, page_insert, page2kva and e->env_pgdir
	//  You must figure out which permissions you'll need
	//  for the different mappings you create.
	//  Remember that the binary image is an elf format image,
	//  which contains both text and data.

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

	//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
}

//
// Allocates a new env and loads the a.out binary into it.
//  - new env's parent env id is 0
void
env_create(u_char *binary, int size)
{
	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

	//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
}


//
// Frees env e and all memory it uses.
// 
void
env_free(struct Env *e)
{
	Pte *pt;
	u_int pdeno, pteno, pa;

	// Note the environment's demise.
	printf("[%08x] free env %08x\n", curenv ? curenv->env_id : 0, e->env_id);

	// Flush all pages

	for (pdeno = 0; pdeno < PDX(UTOP); pdeno++) {
		if (!(e->env_pgdir[pdeno] & PTE_V))
			continue;
		pa = PTE_ADDR(e->env_pgdir[pdeno]);
		pt = (Pte*)KADDR(pa);
		for (pteno = 0; pteno <= PTX(~0); pteno++)
			if (pt[pteno] & PTE_V)
				page_remove(e->env_pgdir, (pdeno << PDSHIFT) | (pteno << PGSHIFT));
		e->env_pgdir[pdeno] = 0;
		page_decref(pa2page(pa));
	}
	pa = e->env_cr3;
	e->env_pgdir = 0;
	e->env_cr3 = 0;
	page_decref(pa2page(pa));


	e->env_status = ENV_FREE;
	LIST_INSERT_HEAD(&env_free_list, e, env_link);
}

// Frees env e.  And schedules a new env
// if e is the current env.
//
void
env_destroy(struct Env *e) 
{
	env_free(e);
	if (curenv == e) {
		curenv = NULL;
		//panic("bcopy(): src:%x  dst:%x ",(int)KERNEL_SP-sizeof(struct Trapframe),TIMESTACK-sizeof(struct Trapframe));
		bcopy((int)KERNEL_SP-sizeof(struct Trapframe),TIMESTACK-sizeof(struct Trapframe),sizeof(struct Trapframe));
		printf("i am killed ... \n");
		sched_yield();
	}
}


//
// Restores the register values in the Trapframe
//  (does not return)
//
extern void env_pop_tf(struct Trapframe *tf,int id);
extern void lcontext(u_int contxt);

//
// Context switch from curenv to env e.
// Note: if this is the first call to env_run, curenv is NULL.
//  (This function does not return.)
//
void
env_run(struct Env *e)
{
	struct Trapframe*  old;
	old=(struct Trapframe*)(TIMESTACK-sizeof(struct Trapframe));
	if(curenv)
	{	
		bcopy(old,&curenv->env_tf,sizeof(struct Trapframe));
		curenv->env_tf.pc=old->cp0_epc;
	}
	curenv = e;
	
	lcontext(KADDR(curenv->env_cr3));
	
	env_pop_tf(&(curenv->env_tf),GET_ENV_ASID(curenv->env_id));

	panic("there is never reached otherwise error !\n");
}

