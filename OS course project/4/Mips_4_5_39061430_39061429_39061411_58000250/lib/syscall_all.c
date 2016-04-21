#include "../drivers/gxconsole/dev_cons.h"
#include <mmu.h>
#include <env.h>
#include <printf.h>
#include <pmap.h>

extern char * KERNEL_SP;
extern struct Env * curenv;


void sys_putchar(int sysno,int c,int a2,int a3,int a4,int a5)
{
 	printcharc((char) c);
	return ;
}


void * memcpy(void *destaddr,void const *srcaddr,u_int len)
{
	char *dest = destaddr;
	char const *src = srcaddr;

	while(len-->0)
		*dest++=*src++;
	return destaddr;
}


// return the current environment id
u_int sys_getenvid(void)
{
	return curenv->env_id;
}


// deschedule current environment
void sys_yield(void)
{
	bcopy((int)KERNEL_SP-sizeof(struct Trapframe),TIMESTACK-sizeof(struct Trapframe),sizeof(struct Trapframe));
	sched_yield();
}

// destroy the current environment
void sys_env_destroy(int sysno,u_int envid)
{
/*
	printf("[%08x] exiting gracefully\n", curenv->env_id);
	env_destroy(curenv);
*/
	int r;
	struct Env *e;

	if ((r=envid2env(envid, &e, 1)) < 0)
		return r;
	printf("[%08x] destroying %08x\n", curenv->env_id, e->env_id);
	env_destroy(e);
	return 0;
}


// Set envid's pagefault handler entry point and exception stack.
// (xstacktop points one byte past exception stack).
//
// Returns 0 on success, < 0 on error.
int sys_set_pgfault_handler(int sysno,u_int envid, u_int func, u_int xstacktop)
{
    ///////////////////////////////////////////////////////
    //your code here
	struct Env *e;
	if( envid2env(envid, &e, 1) < 0)
		panic("sys_set_pgfault_handler not implemented");	
	//printf("set pgfault handler\n");
	e->env_pgfault_handler = func;
	xstacktop = TRUP(xstacktop);
	e->env_xstacktop = xstacktop;
	return 0;
    //
    ///////////////////////////////////////////////////////
  	
}

//
// Allocate a page of memory and map it at 'va' with permission
// 'perm' in the address space of 'envid'.
//
// If a page is already mapped at 'va', that page is unmapped as a
// side-effect.
//
// perm -- PTE_U|PTE_P are required, 
//         PTE_AVAIL|PTE_W are optional,
//         but no other bits are allowed (return -E_INVAL)
//
// Return 0 on success, < 0 on error
//	- va must be < UTOP
//	- env may modify its own address space or the address space of its children
// 
int sys_mem_alloc(int sysno,u_int envid, u_int va, u_int perm)
{
    ///////////////////////////////////////////////////////
    //your code here
	struct Env *e;
	struct Page *p;
	if(envid2env(envid, &e, 1) < 0)
		//panic("sys_mem_alloc not implemented");
		return -1;
	if(page_alloc(&p) < 0)
		//panic("sys_mem_alloc not implemented");
		return -1;
	//if(!(perm & PTE_U) || !(perm & PTE_P))
	//	panic("sys_mem_alloc not implemented");
	//memset(page2kva(p), 0 ,BY2PG);
	bzero((void*)page2kva(p),BY2PG);
	if(page_insert(e->env_pgdir, p, va, perm) < 0){
		//page_free(p);
		//panic("sys_mem_alloc not implemented");
		return -1;
	}
	return 0;
    //
    ///////////////////////////////////////////////////////

}

// Map the page of memory at 'srcva' in srcid's address space
// at 'dstva' in dstid's address space with permission 'perm'.
// Perm has the same restrictions as in sys_mem_alloc.
// (Probably we should add a restriction that you can't go from
// non-writable to writable?)
//
// Return 0 on success, < 0 on error.
//
// Cannot access pages above UTOP.
int sys_mem_map(int sysno,u_int srcid, u_int srcva, u_int dstid, u_int dstva, u_int perm)
{
    ///////////////////////////////////////////////////////
    //your code here
	struct Env *srcenv, *dstenv;
	struct Page *page;
	Pte *srcpte, *dstpte;

	if (envid2env(srcid, &srcenv, 1) < 0 ||
		envid2env(dstid, &dstenv, 1) < 0)
		//panic("sys_mem_map not implemented 1");
		return -1;

 	srcva = ROUNDDOWN(srcva, BY2PG);
	dstva = ROUNDDOWN(dstva, BY2PG);
	if ((unsigned int)srcva >= UTOP || srcva != ROUNDDOWN(srcva, BY2PG) ||
		(unsigned int)dstva >= UTOP || dstva != ROUNDDOWN(dstva, BY2PG))
		//panic("sys_mem_map not implemented 2");
		return -1;

	if ((page = page_lookup(srcenv->env_pgdir, srcva, &srcpte)) == NULL)
		//panic("sys_mem_map not implemented 3");
		return -1;
	// PTE_U and PTE_P must be set
	/*if (!(perm & PTE_U) || !(perm & PTE_P))
		panic("sys_mem_map not implemented");
	// other bits than PTE_{U,P,W,AVAIL} are set
	if (perm & ((~(PTE_U | PTE_P | PTE_W | PTE_AVAIL)) & 0xfff))
		panic("sys_mem_map not implemented");
	// perm has PTE_W, but scrpte is read-only.
	if ((perm & PTE_W) && !(*srcpte & PTE_W))
		panic("sys_mem_map not implemented");
	*/
	//fmars
	//printf("%x ",dstva);
	if (page_insert(dstenv->env_pgdir, page, dstva, perm) != 0)
		//panic("sys_mem_map not implemented 4");
		return -1;
	/*cprintf("map [%08x] %08x(%08x) -> [%08x] %08x(%08x) perm: %x\n",
			srcenv->env_id, srcva, *srcpte,
			dstenv->env_id, dstva, *dstpte, perm);*/

	return 0;
    ///////////////////////////////////////////////////////
	//	panic("sys_mem_map not implemented");
}

// Unmap the page of memory at 'va' in the address space of 'envid'
// (if no page is mapped, the function silently succeeds)
//
// Return 0 on success, < 0 on error.
//
// Cannot unmap pages above UTOP.
int sys_mem_unmap(int sysno,u_int envid, u_int va)
{
    ///////////////////////////////////////////////////////
    //your code here
	struct Env *task;

	if (envid2env(envid, &task, 1) < 0)
		panic("sys_mem_unmap 1");

	if ((unsigned int)va >= UTOP || va != ROUNDDOWN(va, BY2PG))
		panic("sys_mem_unmap 2 UTOP %x va %x %x",UTOP, va, ROUNDDOWN(va, BY2PG));

	page_remove(task->env_pgdir, va);

	return 0;    
    ///////////////////////////////////////////////////////
}

// Allocate a new environment.
//
// The new child is left as env_alloc created it, except that
// status is set to ENV_NOT_RUNNABLE and the register set is copied
// from the current environment.  In the child, the register set is
// tweaked so sys_env_alloc returns 0.
//
// Returns envid of new environment, or < 0 on error.
int sys_env_alloc(void)
{
    ///////////////////////////////////////////////////////
    //your code here
    //
	struct Env *child;

	if (env_alloc(&child, curenv->env_id) < 0)
		return -E_NO_FREE_ENV;

	//child->env_status = ENV_NOT_RUNNABLE;
	//child->env_tf = curenv->env_tf;
	bcopy(KERNEL_SP - sizeof(struct Trapframe), &child->env_tf,sizeof(struct Trapframe));	
	// install the pgfault upcall to the child
	child->env_status = ENV_NOT_RUNNABLE;
	child->env_pgfault_handler = 0;
	child->env_tf.pc = child->env_tf.cp0_epc;
	// tweak the register eax of the child,
	// thus, the child will look like the return value
	// of the the system call is zero.

	child->env_tf.regs[2] = 0;

	//fmars try
	//child->env_pgdir = curenv->env_pgdir;
	//child->env_cr3 = curenv->env_cr3;
	// but notice that the return value of the parent
	// is the env id of the child
	//fmars printf("env_alloc -> child_envid =%d parent_envid=%d\n",child->env_id,curenv->env_id);
	return child->env_id;
}

// Set envid's env_status to status. 
//
// Returns 0 on success, < 0 on error.
// 
// Return -E_INVAL if status is not a valid status for an environment.
int sys_set_env_status(int sysno,u_int envid, u_int status)
{
    ///////////////////////////////////////////////////////
    //your code here
	int r;
	struct Env *task;

	if ((r = envid2env(envid, &task, 1)) < 0)
		panic("sys_env_set_status not implemented");

	if (status != ENV_FREE &&
		status != ENV_RUNNABLE &&
		status != ENV_NOT_RUNNABLE)
		panic("sys_env_set_status not implemented");

	task->env_status = status;

	return 0;
    ///////////////////////////////////////////////////////
	//panic("sys_env_set_status not implemented");
}

// Set envid's trap frame to tf.
//
// Returns 0 on success, < 0 on error.
//
// Return -E_INVAL if the environment cannot be manipulated.
int sys_set_trapframe(int sysno,u_int envid, struct Trapframe *tf)
{
	int r;
	struct Env *task;

	if ((r = envid2env(envid, &task, 1)) < 0)
		return -E_BAD_ENV;

	task->env_tf = *tf;

	return 0;
}

void sys_panic(int sysno,char *msg)
{
	// no page_fault_mode -- we are trying to panic!
	panic("%s", TRUP(msg));
}


// Block until a value is ready.  Record that you want to receive,
// mark yourself not runnable, and then give up the CPU.
void sys_ipc_recv(int sysno,u_int dstva)
{
    ///////////////////////////////////////////////////////
    //your code here
    //
	if ((unsigned int)dstva >= UTOP || dstva != ROUNDDOWN(dstva, BY2PG))
		return -E_INVAL;

	curenv->env_ipc_dstva = dstva;
	curenv->env_ipc_recving = 1;
	//curenv->env_status = ENV_NOT_RUNNABLE;
	// set the return value to be zero,
	// it is necessary, because the 'return' statement
	// after 'sched_yield' will never be executed,
	// actually it is skipped.
	//curenv->env_tf.tf_regs.reg_eax = 0;
	// give up the CPU
	//sched_yield();
	return 0;

    ///////////////////////////////////////////////////////
}

// Try to send 'value' to the target env 'envid'.
//
// The send fails with a return value of -E_IPC_NOT_RECV if the
// target has not requested IPC with sys_ipc_recv.
//
// Otherwise, the send succeeds, and the target's ipc fields are
// updated as follows:
//    env_ipc_recving is set to 0 to block future sends
//    env_ipc_from is set to the sending envid
//    env_ipc_value is set to the 'value' parameter
// The target environment is marked runnable again.
//
// Return 0 on success, < 0 on error.
//
// Hint: the only function you need to call is envid2env.
int sys_ipc_can_send(int sysno,u_int envid, u_int value, u_int srcva, u_int perm)
{
    ///////////////////////////////////////////////////////
    //your code here
    //
	struct Env *target;
	struct Page *page;
	Pte *pte;
	int r, ret = 0;

	if ((r = envid2env(envid, &target, 0)) < 0)
		return -E_BAD_ENV;

	if (!target->env_ipc_recving)
		return -E_IPC_NOT_RECV;

	// srcva is not null, then
	// we need to map it, thus sharing the map
	if (srcva) {
		if ((unsigned int)srcva >= UTOP)
			return -E_INVAL;

		if (srcva != ROUNDDOWN(srcva, BY2PG))
			return -E_INVAL;

		if ((page = page_lookup(curenv->env_pgdir, srcva, &pte)) == NULL)
			return -E_INVAL;

		// PTE_U and PTE_P must be set
		/*if (!(perm & PTE_U) || !(perm & PTE_P))
			return -E_INVAL;
		// other bits than PTE_{U,P,W,AVAIL} are set
		if (perm & ((~(PTE_U | PTE_P | PTE_W | PTE_AVAIL)) & 0xfff))
			return -E_INVAL;
		// perm has PTE_W, but scrpte is read-only.
		if ((perm & PTE_W) && !(*pte & PTE_W))
			return -E_INVAL;*/

		if (page_insert(target->env_pgdir, page, target->env_ipc_dstva, perm) < 0)
			return -E_NO_MEM;

		ret = 1;
	}

	target->env_ipc_recving = 0;
	target->env_ipc_value = value;
	//printf("target value %d\n",value);
	target->env_ipc_from = curenv->env_id;
	if (ret)
		target->env_ipc_perm = perm;
	else
		target->env_ipc_perm = 0;
	//target->env_status = ENV_RUNNABLE;

	return ret;    ///////////////////////////////////////////////////////
}
