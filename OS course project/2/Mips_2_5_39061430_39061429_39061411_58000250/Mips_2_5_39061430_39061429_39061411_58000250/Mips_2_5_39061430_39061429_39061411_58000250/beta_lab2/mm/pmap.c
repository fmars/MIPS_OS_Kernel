#include "mmu.h"
#include "pmap.h"
#include "printf.h"
#include "env.h"
#include "error.h"



/* These variables are set by mips_detect_memory() */
u_long maxpa;            /* Maximum physical address */
u_long npage;            /* Amount of memory(in pages) */
u_long basemem;          /* Amount of base memory(in bytes) */
u_long extmem;           /* Amount of extended memory(in bytes) */

Pde* boot_pgdir;

struct Page *pages;
static u_long freemem;	/* (Pointer to next byte of free mem) */

static struct Page_list page_free_list;	/* Free list of physical pages */

void mips_detect_memory()
{
	
	basemem = 64*1024*1024;
	extmem = 0;
	maxpa = basemem;

	npage = maxpa / BY2PG;//16k��ҳ��

	printf("Physical memory: %dK available, ", (int)(maxpa/1024));
	printf("base = %dK, extended = %dK\n", (int)(basemem/1024), (int)(extmem/1024));
}

static void * alloc(u_int n, u_int align, int clear)
{
	extern char end[]; 	u_long alloced_mem;
	// initialize freemem if this is the first time
	if (freemem == 0)
		freemem = (u_long)end;//(end���׵�ַ��mmu.h�б�ʾ��0x8040 0000)

	printf("pmap.c:\talloc from %x",freemem);
	///////////////////////////////////////////////////////////////////
	// Your code here:
	//	Step 1: round freemem up to be aligned properly
	freemem = ROUND(freemem, align);//(����align�Ĵ�С����)
	//	Step 2: save current value of freemem as allocated chunk
	alloced_mem = freemem;//(���浱ǰֵ)
	//	Step 3: increase freemem to record allocation
	freemem += n;//(����)
	//	Step 4: clear allocated chunk if necessary
	if (clear)
		bzero((void*)alloced_mem, n);//(���clearΪ1��������ռ���0)
	//	Step 5: return allocated chunk
	return (void*)alloced_mem;//(����)
	//v?????????????????
	
	//A?????????????????
}


//
// Given pgdir, the current page directory base,
// walk the 2-level page table structure to find
// the Pte for virtual address va.  Return a pointer to it.
//
// If there is no such directory page:
//	- if create == 0, return 0.
//	- otherwise allocate a new directory page and install it.
//
// This function is abstracting away the 2-level nature of
// the page directory for us by allocating new page tables
// as needed.
// 
// Boot_pgdir_walk cannot fail.  It's too early to fail.
// 
static Pte* boot_pgdir_walk(Pde *pgdir, u_long va, int create)//(����va��Ӧҳ�����ָ��)
{
	//v?????????????????
	Pde *pde;//(page directory�����ڱ���ҳĿ¼��)
	Pte *pgtab;//(virtual page table�����ڱ�����ҳ��ĵ�ַ)
	pde = &pgdir[PDX(va)];//(PDX����22λ��ҳĿ¼�pgdir+ƫ�����õ�ҳĿ¼���ַ)
	
	if (!(*pde & PTE_V)){//(���û�з���)
		if (!create)//	- if create == 0, return 0.
			return 0;
		pgtab = alloc(BY2PG, BY2PG, 1);//	- otherwise allocate a new directory page and install it.
		*pde = PTE_ADDR(PADDR(pgtab))|PTE_V;//(�޸�ҳĿ¼��ָ�������ҳ��ַ������ռ䣬���޸ı��λ)
	}
	pgtab = (Pte*)KADDR(PTE_ADDR(*pde));//(��ҳĿ¼���ҵ�ҳ���ַ�����������ʾ������Ϣ)
	return &pgtab[PTX(va)];//(��va��ҳ����ָ�뷵��)
	//A?????????????????
}

//
// Map [va, va+size) of virtual address space to physical [pa, pa+size)
// in the page table rooted at pgdir.  Size is a multiple of BY2PG.
// Use permission bits perm|PTE_V for the entries.
//
void boot_map_segment(Pde *pgdir, u_long va, u_long size, u_long pa, int perm)
{
	//v?????????????????
	u_long i;//(����һ���ֲ���������ѭ��)
  
	for (i=0; i<size; i+=BY2PG) {
		*boot_pgdir_walk(pgdir, va+i, 1) = (pa+i)|perm|PTE_V;//*(���ÿһ��ҳ�����дvaָ������ַ��Ӧ�������ַpa)
	}
	//A?????????????????
}

// Set up a two-level page table:
// 
// This function only sets up the kernel part of the address space
// (ie. addresses >= UTOP).  The user part of the address space
// will be setup later.
//
void mips_vm_init()
{
	extern char KVPT[];
	extern char end[];
	extern int mCONTEXT;
	extern struct Env *envs;
	struct Env *en;

	Pde* pgdir;
	u_int n;


	//1.allocate memory for Page directory
	pgdir = alloc(BY2PG, BY2PG, 1);//(allocate a new directory page and install it.)
	//2.intitialize mCONTEXT,boot_pgdir(ҳĿ¼)
	boot_pgdir = pgdir;
	mCONTEXT=(int)pgdir;
	//3.allocate memory for Page structures,this structure is used to record the physical page info
		//(0x7f80 0000 - 0x7fc0 0000 PDMAP)
	if ((pages = alloc((sizeof(struct Page) * npage), BY2PG, 1)) == 0) {
	  panic("Couldn't allocate space for pages\n");
	}
	boot_map_segment(pgdir, UPAGES, (sizeof(struct Page) * npage), PADDR((u_long)pages), PTE_V);
	
	//4.allocate memory for ENV.
	//(0x7f40 0000 - 0x7f80 0000 PDMAP)
	if ((en = alloc((sizeof(struct Env) * NENV), BY2PG, 1)) == 0) {
	  panic("Couldn't allocate space for pages\n");
	}
	envs = en;
	boot_map_segment(pgdir, UENVS, (sizeof(struct Env) * NENV), PADDR((u_long)en), PTE_V);

	//v?????????????????

	//A?????????????????
	check_boot_pgdir();//(���ҳ�����)
}

static void
check_boot_pgdir(void)
{
	u_long i, n;
	Pde *pgdir;

	pgdir = boot_pgdir;

	// check pages array
	n = ROUND(npage*sizeof(struct Page), BY2PG);
	for(i=0; i<n; i+=BY2PG)
		assert(va2pa(pgdir, UPAGES+i) == PADDR(pages)+i);
	
	// check envs array
	n = ROUND(NENV*sizeof(struct Env), BY2PG);
	for(i=0; i<n; i+=BY2PG)
		assert(va2pa(pgdir, UENVS+i) == PADDR(envs)+i);

	printf("check_boot_pgdir() succeeded!\n");
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------
// Tracking of physical pages.
// -
static void page_initpp(struct Page *pp);
void
page_init(void)
{
	// The exaple code here marks all pages as free.
	// However this is not truly the case.  What memory is free?
	//  1) Mark page 0 as in use(for good luck) 
	//  2) Mark the rest of base memory as free.
	//  3) Then comes the IO hole [IOPHYSMEM, EXTPHYSMEM) => mark it as in use
	//     So that it can never be allocated.      
	//  4) Then extended memory(ie. >= EXTPHYSMEM):
	//     ==> some of it's in use some is free. Where is the kernel?
	//     Which pages are used for page tables and other data structures?    
	//
	// Change the code to reflect this.
	//v?????????????????
	
	int i, inuse;//(mark of in use)

	LIST_INIT (&page_free_list);//(��δʹ������ҳ�б��ʼ��)
	alloc(0, BY2PG, 0);//(���ڴ水��ҳ��С����)
	
	for (i=0; i<npage; i++) {//�ȱ��Ϊδ��
		pages[i].pp_ref=0;
		LIST_INSERT_HEAD(&page_free_list,&pages[i],pp_link);
	}
	pages[0].pp_ref=1;//��0ȥ��
	LIST_REMOVE(&pages[0],pp_link);
	for (i=PADDR(KERNBASE)/BY2PG; i<ROUND(PADDR((u_int)freemem),BY2PG)/BY2PG; i++) {
		pages[i].pp_ref=1;
		LIST_REMOVE(&pages[i],pp_link);
	}//ȥ�����õĲ���
	//A?????????????????
	
}

//
// Initialize a Page structure.
//
static void
page_initpp(struct Page *pp)
{
	bzero(pp, sizeof(*pp));
}


//
// Allocates a physical page.
//
// *pp -- is set to point to the Page struct of the newly allocated
// page
//
// RETURNS 
//   0 -- on success
//   -E_NO_MEM -- otherwise 
//
// Hint: use LIST_FIRST, LIST_REMOVE, page_initpp()
// Hint: pp_ref should not be incremented 
int
page_alloc(struct Page **pp)
{
	// Fill this function in
	//v?????????????????
	if ((*pp = LIST_FIRST(&page_free_list)) == NULL) {//(�����������ҳ�б�Ϊ�գ�˵������)
		return -E_NO_MEM;//(����ʧ��)
	}

	LIST_REMOVE(*pp, pp_link);//(�ڿ�������ҳ�б����������)
	page_initpp(*pp);//(��յ�ǰPage����)
	bzero((Pte *)page2kva(*pp), BY2PG);//(���ں����ַ��Ӧ�������)///no use???
	return 0;//(����ɹ�)
	//A?????????????????
}


//
// Return a page to the free list.
// (This function should only be called when pp->pp_ref reaches 0.)
//
void
page_free(struct Page *pp)
{
	// Fill this function in
	//v?????????????????
	if(pp->pp_ref>0)//(�ж�pp->pp_ref�Ƿ�Ϊ0)
	{
		panic("page_free: error!\n");
		return ;
	}
	LIST_INSERT_HEAD(&page_free_list, pp, pp_link);//(�ͷ��Ժ�����ӵ���������ҳ�б��ͷ��)
	//A?????????????????
}



void
page_decref(struct Page *pp)
{
	if (--pp->pp_ref == 0)
		page_free(pp);
}


//
// This is boot_pgdir_walk with a different allocate function.
// Unlike boot_pgdir_walk, pgdir_walk can fail, so we have to
// return pte via a pointer parameter.
//
// Stores address of page table entry in *pte.
// Stores 0 if there is no such entry or on error.
// 
// RETURNS: 
//   0 on success
//   -E_NO_MEM, if page table couldn't be allocated
//
int
pgdir_walk(Pde *pgdir, u_long va, int create, Pte **ppte)
{
	// Fill this function in
	//v?????????????????
	int r;
	struct Page *pp;
	Pde *pde;
	Pte *pgtab;

	pde = (Pde*)&pgdir[PDX(va)];//(����va��Ӧ��ҳĿ¼��)
	if(!(*pde&PTE_V))//�����Ч��������ռ�
	{
		if((create==0)||(page_alloc(&pp)<0))//ʧ�ܷ���-E_NO_MEM
			return -E_NO_MEM;
		pp->pp_ref=1;//�޸�ҳ������Ϣ
		bzero((void*)(KADDR(page2pa(pp))),BY2PG);//�����ҳ
		*pde=page2pa(pp)|PTE_V; //�޸�ҳĿ¼����Ϣ
	}
	pgtab=(Pte*)(KADDR(PTE_ADDR(*pde)));//(�����·����ҳ����)
	*ppte=&pgtab[PTX(va)];//(��*ppteָ��va����Ӧ��ҳ����)
	
	//A?????????????????
	return 0;
}

void
tlb_invalidate(Pde *pgdir, u_long va)
{
	if (curenv)
		tlb_out(PTE_ADDR(va)|GET_ENV_ASID(curenv->env_id));
	else
		tlb_out(PTE_ADDR(va));

}


//
// Map the physical page 'pp' at virtual address 'va'.
// The permissions (the low 12 bits) of the page table
//  entry should be set to 'perm|PTE_P'.(*PTE_V*)
//
// Details
//   - If there is already a page mapped at 'va', it is page_remove()d.
//   - If necesary, on demand, allocates a page table and inserts it into 'pgdir'.
//   - pp->pp_ref should be incremented if the insertion succeeds
//
// RETURNS: 
//   0 on success
//   -E_NO_MEM, if page table couldn't be allocated
//
// Hint: The TA solution is implemented using
//   pgdir_walk() and and page_remove().
//
int
page_insert(Pde *pgdir, struct Page *pp, u_long va, u_int perm) 
{
	// Fill this function in
	//v?????????????????
	int r,flag=0;
	Pte *pte;
  
	if ((r = pgdir_walk(pgdir, va, 1, &pte)) < 0)//(����ҳĿ¼�ʧ�ܷ���-E_NO_MEM)
		return r;
	if(*pte&PTE_V)
	{
        if(pa2page(PTE_ADDR(*pte))!=pp)
		{
    		page_remove(pgdir,va);//(��ȡ��ӳ���ϵ)
			pp->pp_ref++;//(�ɹ����޸�ҳ����)
        }    
        *pte=page2pa(pp)|perm|PTE_V; //(�޸�ҳ����ı��λ)
		flag=1;   
    }
   	else
	{
        *pte=page2pa(pp)|perm|PTE_V;//(�޸�ҳ����ı��λ)
        pp->pp_ref++;//(�ɹ����޸�ҳ����)
    }
	if(flag)
		tlb_invalidate(pgdir,va);

	//A?????????????????
	return 0;
}



struct Page*
page_lookup(Pde *pgdir, u_long va, Pte **ppte)
{
	struct Page *ppage;
	Pte *pte;

        pgdir_walk(pgdir, va, 0, &pte);
       	if(pte==0) return 0;
       	if((*pte & PTE_V)==0) return 0;       //the page is not in memory.
       	ppage = pa2page(*pte);
	if(ppte) *ppte = pte;
	return ppage;
}


//
// Unmaps the physical page at virtual address 'va'.
//
// Details:
//   - The ref count on the physical page should decrement.
//   - The physical page should be freed if the refcount reaches 0.
//   - The pg table entry corresponding to 'va' should be set to 0.
//     (if such a PTE exists)
//   - The TLB must be invalidated if you remove an entry from
//	   the pg dir/pg table.
//
// Hint: The TA solution is implemented using
//  pgdir_walk(), page_free(), tlb_invalidate()
//

void
page_remove(Pde *pgdir, u_long va) 
{
	// Fill this function in
	//v?????????????????
	Pte *pte;
	struct Page *pp;
	
	pp=page_lookup(pgdir,va,&pte);//(����ҳĿ¼��)
	
	if(pp==NULL)//(�������򷵻�) 
		return;
		
	page_decref(pp);//(pp->pp_ref--��page_free(pp);)
	
	if(pte!=NULL) 
		*pte=0;
		
	tlb_invalidate(pgdir,va);//(�޸ı��λ)
	
	//A?????????????????
	return;
}



void
page_check(void)
{
	struct Page *pp, *pp0, *pp1, *pp2;
	struct Page_list fl;

	// should be able to allocate three pages
	pp0 = pp1 = pp2 = 0;
	assert(page_alloc(&pp0) == 0);
	assert(page_alloc(&pp1) == 0);
	assert(page_alloc(&pp2) == 0);

	assert(pp0);
	assert(pp1 && pp1 != pp0);
	assert(pp2 && pp2 != pp1 && pp2 != pp0);

	// temporarily steal the rest of the free pages
	fl = page_free_list;
	LIST_INIT(&page_free_list);

	// should be no free memory
	assert(page_alloc(&pp) == -E_NO_MEM);

	// there is no free memory, so we can't allocate a page table 
	assert(page_insert(boot_pgdir, pp1, 0x0, 0) < 0);

	// free pp0 and try again: pp0 should be used for page table
	page_free(pp0);
	assert(page_insert(boot_pgdir, pp1, 0x0, 0) == 0);
	assert(PTE_ADDR(boot_pgdir[0]) == page2pa(pp0));
	assert(va2pa(boot_pgdir, 0x0) == page2pa(pp1));
	assert(pp1->pp_ref == 1);

	// should be able to map pp2 at BY2PG because pp0 is already allocated for page table
	assert(page_insert(boot_pgdir, pp2, BY2PG, 0) == 0);
	assert(va2pa(boot_pgdir, BY2PG) == page2pa(pp2));
	assert(pp2->pp_ref == 1);
	// should be no free memory
	assert(page_alloc(&pp) == -E_NO_MEM);
//printf("why\n");
	// should be able to map pp2 at BY2PG because it's already there
	assert(page_insert(boot_pgdir, pp2, BY2PG, 0) == 0);
	assert(va2pa(boot_pgdir, BY2PG) == page2pa(pp2));
	assert(pp2->pp_ref == 1);
//printf("It is so unbelivable\n");
	// pp2 should NOT be on the free list
	// could happen in ref counts are handled sloppily in page_insert
	assert(page_alloc(&pp) == -E_NO_MEM);

	// should not be able to map at PDMAP because need free page for page table
	assert(page_insert(boot_pgdir, pp0, PDMAP, 0) < 0);

	// insert pp1 at BY2PG (replacing pp2)
	assert(page_insert(boot_pgdir, pp1, BY2PG, 0) == 0);

	// should have pp1 at both 0 and BY2PG, pp2 nowhere, ...
	assert(va2pa(boot_pgdir, 0x0) == page2pa(pp1));
	assert(va2pa(boot_pgdir, BY2PG) == page2pa(pp1));
	// ... and ref counts should reflect this
	assert(pp1->pp_ref == 2);
	assert(pp2->pp_ref == 0);

	// pp2 should be returned by page_alloc
	assert(page_alloc(&pp) == 0 && pp == pp2);

	// unmapping pp1 at 0 should keep pp1 at BY2PG
	page_remove(boot_pgdir, 0x0);
	assert(va2pa(boot_pgdir, 0x0) == ~0);
	assert(va2pa(boot_pgdir, BY2PG) == page2pa(pp1));
	assert(pp1->pp_ref == 1);
	assert(pp2->pp_ref == 0);

	// unmapping pp1 at BY2PG should free it
	page_remove(boot_pgdir, BY2PG);
	assert(va2pa(boot_pgdir, 0x0) == ~0);
	assert(va2pa(boot_pgdir, BY2PG) == ~0);
	assert(pp1->pp_ref == 0);
	assert(pp2->pp_ref == 0);

	// so it should be returned by page_alloc
	assert(page_alloc(&pp) == 0 && pp == pp1);

	// should be no free memory
	assert(page_alloc(&pp) == -E_NO_MEM);

	// forcibly take pp0 back
	assert(PTE_ADDR(boot_pgdir[0]) == page2pa(pp0));
	boot_pgdir[0] = 0;
	assert(pp0->pp_ref == 1);
	pp0->pp_ref = 0;

	// give free list back
	page_free_list = fl;

	// free the pages we took
	page_free(pp0);
	page_free(pp1);
	page_free(pp2);

	printf("page_check() succeeded!\n");
}



void pageout(int va,int context)
{
	u_long r;
	struct Page *p=NULL;

	if (context<0x80000000)
		panic("tlb refill and alloc error!");
	if ((va>0x7f400000)&&(va<0x7f800000))
		panic(">>>>>>>>>>>>>>>>>>>>>>it's env's zone");

	if (va<0x10000)
		panic("^^^^^^TOO LOW^^^^^^^^^"); 
	if ((r = page_alloc(&p)) < 0)
		panic ("page alloc error!");
	p->pp_ref++;

	page_insert((int*)context, p, VA2PFN(va), PTE_R);
	printf("pageout:\t@@@___0x%x___@@@  ins a page \n",va);
}

