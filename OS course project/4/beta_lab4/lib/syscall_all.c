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
    //
    ///////////////////////////////////////////////////////
  	panic("sys_set_pgfault_handler not implemented");
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
    //
    ///////////////////////////////////////////////////////
	panic("sys_mem_alloc not implemented");
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
    //
    ///////////////////////////////////////////////////////
	panic("sys_mem_map not implemented");
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
    //
    ///////////////////////////////////////////////////////
	panic("sys_mem_unmap not implemented");
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
    ///////////////////////////////////////////////////////
	panic("sys_env_alloc not implemented");
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
    //
    ///////////////////////////////////////////////////////
	panic("sys_env_set_status not implemented");
}

// Set envid's trap frame to tf.
//
// Returns 0 on success, < 0 on error.
//
// Return -E_INVAL if the environment cannot be manipulated.
int sys_set_trapframe(int sysno,u_int envid, struct Trapframe *tf)
{
	
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
    ///////////////////////////////////////////////////////
}
