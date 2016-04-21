// implement fork from user space

#include "lib.h"
#include <mmu.h>
#include <env.h>

//fmars define a free mem space for pgfault
#define PFTEMP (ROUNDDOWN(0x50000000, BY2PG))//( USTACKTOP - 2 * BY2PG )
//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//

void user_bcopy(const void *src, void *dst, size_t len)
{
	void *max;

	//	writef("~~~~~~~~~~~~~~~~ src:%x dst:%x len:%x\n",(int)src,(int)dst,len);
	max = dst + len;
	// copy machine words while possible
	if (((int)src%4==0)&&((int)dst%4==0))
	{
		while (dst + 3 < max)
		{
			*(int *)dst = *(int *)src;
			dst+=4;
			src+=4;
		}
	}
	
	// finish remaining 0-3 bytes
	while (dst < max)
	{
		*(char *)dst = *(char *)src;
		dst+=1;
		src+=1;
	}
	//for(;;);
}


void user_bzero(void *v, u_int n)
{
	char *p;
	int m;

	p = v;
	m = n;
	while (--m >= 0)
		*p++ = 0;
}


//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//

static void
pgfault(u_int va)
{
	int r;
	int i;
	u_char *tmp;
	va = ROUNDDOWN(va, BY2PG);
	writef("fmars pgfault envid = %x va = %x TEMP = %x\n", syscall_getenvid(), va, PFTEMP);
	if (!((*vpt)[VPN(va)] & PTE_COW ))
		user_panic("not a write and not to a COW page %x\n", (*vpt)[VPN(va)]);
	//writef("pgfault 1 va = %x",va);
	if (syscall_mem_alloc(0, PFTEMP,  PTE_R  ) < 0)
		user_panic("sys_page_alloc\n");
	//writef("pgfault 2 va = %x",va);
	//memmove((void*)PFTEMP, ROUNDDOWN(addr, PGSIZE), PGSIZE);
	user_bcopy((void*)va, PFTEMP, BY2PG);
	//writef("pgfault 2.5 va = %x",va);
	//writef("printff%x %x %x\n",*((int*)va),*((int*)va+1),*((int*)va+2));
	for(r=0;i<1020;i++)
		if( *((int*)va) != *((int*)PFTEMP) )
			writef("%d",*((int*)va));
	if( syscall_mem_map(0, PFTEMP, 0, va, PTE_R  ) < 0)
		user_panic("sys_page_map\n");
	//writef("pgfault 3 va = %x",va);
	if( syscall_mem_unmap(0, PFTEMP) < 0)
		user_panic("sys_page_unmap\n");
	writef("va = %x done", va);
		
	//writef("Fmars pgfault\n");
	// Your code here.

	/*
		first page fault va = 7f3fdf70 and this is the normal user's stack
	*/
	//user_panic("\npgfault not implemented pid = %x va = %x\n", syscall_getenvid(), va);
}

//
// Map our virtual page pn (address pn*BY2PG) into the target envid
// at the same virtual address.  if the page is writable or copy-on-write,
// the new mapping must be created copy on write and then our mapping must be
// marked copy on write as well.  (Exercise: why mark ours copy-on-write again if
// it was already copy-on-write?)
// 

/*Fmars Try the asm funciont about *p
void t(){
	int *p,*q;
	*p = 1234;
	
}*/
static void
duppage(u_int envid, u_int pn)
{
	int r;
	u_int addr;
	Pte pte;
	u_int perm;
	//int *p ;
	int i;
	//*p = UVPT;
	perm = (*vpt)[pn] & 0xfff;
	//Fmars
	//writef("importante\n%x %x\n",&(*vpt)[pn],&(*vpt)[pn+1]);
	//writef("importante\n%x %x\n",&(*p)[pn],&(*p)[pn+1]);
	//writef("Duppage %x\n",pn);
	//if( pn == 0x7f3fd )
		//writef("%x\n",perm);
	if( (perm & PTE_R)!=0 || (perm& PTE_COW)!=0) {
		//writef("Duppage Not read-only %x %x\n", pn, perm);
		if(perm&PTE_LIBRARY)
			perm = PTE_V | PTE_R | PTE_LIBRARY;
		else 
			perm = PTE_V|PTE_COW;
		if(syscall_mem_map(0, pn * BY2PG, envid, pn * BY2PG, perm) == -1)
			user_panic("1");
		//writef("Duppage 1 %x %x\n",pn,perm);
		//If has the following sentence , function duppage with pn = 0x7f3fd won't finish, why?
		//syscall_mem_map(0, pn * BY2PG, 0, pn * BY2PG, perm);
	}else{
		if(syscall_mem_map(0, pn * BY2PG, envid, pn * BY2PG, perm) == -1)
			user_panic("2");
		//writef("Duppage     read-only %x %x\n",pn, perm);
	}
	// Your code here.
	//user_panic("duppage not implemented");
	//fmars firstly don't think of the perm
	//syscall_mem_map(syscall_getenvid(), pn * BY2PG, envid, pn*BY2PG, 0x1);
}

//
// User-level fork.  Create a child and then copy our address space
// and page fault handler setup to the child.
//
// Hint: use vpd, vpt, and duppage.
// Hint: remember to fix "env" in the child process!
// 
extern void __asm_pgfault_handler(void);
int
fork(void)
{
	// Your code here.
	u_int envid;
	int pn;
	extern struct Env *envs;
	extern struct Env *env;

	set_pgfault_handler(pgfault);

	if((envid = syscall_env_alloc()) < 0)
		user_panic("err");
	if(envid == 0){
		env = &envs[ENVX(syscall_getenvid())];
		return 0;
	}
	
	pn = UTOP / BY2PG - 1;
	//while(pn-- >= 0){
	for(pn=0; pn<(UTOP / BY2PG) ; pn++){	
		if(((*vpd)[pn/PTE2PT])!=0 && ((*vpt)[pn])!=0){
			//fmars
			//vpd .word (UVPT + (UVPT >> 12) *4 )
			//vpt .word UVPT
			//writef("%x / %x\n",pn,UTOP / BY2PG);
			//writef("pn:%x %x\n",pn*BY2PG,(*vpt)[pn]);
			duppage(envid, pn);
		}
	}
	writef("duppage successful\n");
	if(syscall_mem_alloc(envid, UXSTACKTOP - BY2PG, PTE_V|PTE_R) < 0)
		user_panic("err");
	if(syscall_set_pgfault_handler(envid, __asm_pgfault_handler, UXSTACKTOP) < 0)
		user_panic("err");
	if(syscall_set_env_status(envid, ENV_RUNNABLE) < 0)
		user_panic("err");
	//fmars
	writef("In fork : parent_envid=%x child_envid=%x\n", syscall_getenvid(), envid);
	writef("fork successful!\n");
	return envid;
}

// Challenge!
int
sfork(void)
{
	user_panic("sfork not implemented");
	return -E_INVAL;
}
