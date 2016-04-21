/* See COPYRIGHT for copyright information. */

#include "include/mm.h"
#include "include/env.h"
#include "include/pmap.h"
#include "include/printf.h"

#include "include/lib.h";


LIST_HEAD(Env_list, Env);

struct Env envs[NENV];     // env free list start address
struct Env *curenv = NULL;   // the current env

static struct Env_list env_free_list;	// Free list


//static int KERNEL_SP;
extern char * KERNEL_SP;

extern void env_pop_tf(struct Trapframe *tf);

extern void sched_yield(void);

extern void print_num_hex(unsigned int *n);

extern void print_reg(unsigned int *n, int len);




unsigned int mkenvid(struct Env *e)  // ����һ�����̺�
{
	static unsigned long  next_env_id = 0;
	
	// lower bits of envid hold e's position in the envs array
	unsigned int  idx = e - envs;


	// high bits of envid hold an increasing number
	return (++next_env_id << (1 + LOG2NENV)) | idx; 
}

void
env_init(void)
{
	int i;
	struct Env *e;

	LIST_INIT(&env_free_list);	
	for(i = NENV - 1; i >0; i--) // !! envs[0] reserved for idle process pds----------
	{
		envs[i].env_status = ENV_FREE;
		LIST_INSERT_HEAD(&env_free_list, &envs[i], env_link);

	}

	/*e=env_free_list.lh_first;
	for (i=0;i<NENV && e!=NULL ;i++)
	{
             printf("Free Env List %d addr: 0x%x\n",i,e);
             e=e->env_link.le_next;
         }*/

	printf("env.c:\t env_init() successful!\n");
}


int
env_alloc(struct Env **new)
{
	int id;
	struct Env *e;

	if (!(e = LIST_FIRST(&env_free_list))) 
		return -1;

	e->env_id = mkenvid(e);
	e->env_status = ENV_RUNNABLE;

	id=ENVX(e->env_id);
	e->env_tf.regs[29] = ID2UTOP(id+1)-1;   // �˴��趨��涨���û�����SPջָ��ֵ
	e->env_tf.cp0_status = 0x0003FC00;
 
	LIST_REMOVE(e, env_link);
  	*new=e;

	return 0;
}


static void
load_icode(struct Env *e, u_char *binary, u_int size) 
{
	int id;

	id=ENVX(e->env_id);

	binary+=0x1000;

    e->env_tf.pc=binary-0x4;
    e->env_tf.cp0_epc=binary-0x4;
}


void
env_create(u_char *binary, u_int size)
{
	struct Env *e;


	env_alloc(&e);
	load_icode(e, binary, size);

	printf("Create Env Successful...\n");  // pds----------
}


void
env_destroy(struct Env *e) 
{
	e->env_status = ENV_FREE;
	LIST_INSERT_HEAD(&env_free_list, e, env_link);

	if (curenv == e) {
		curenv = NULL;

		bcopy((int)KERNEL_SP-sizeof(struct Trapframe),TIMESTACK-sizeof(struct Trapframe),sizeof(struct Trapframe));
		
		printf("Env id=d: I am killed ... \n"); //,ENVX(e->env_id));

		sched_yield(); 
	}
}


void
env_run(struct Env *e)
{
	struct Trapframe*  old;
	int id;
	old=(struct Trapframe*)(TIMESTACK-sizeof(struct Trapframe));


	if(curenv)
	{	
		//printf("Save Old epc=0x"); //%x \n",old->cp0_epc);
		//print_num_hex(&(old->cp0_epc));
		printf("\n");
		bcopy(old,&curenv->env_tf,sizeof(struct Trapframe));

		curenv->env_tf.pc=old->cp0_epc;  
	}

	curenv = e;

	//id=ENVX(e->env_id);
	/*printf("Env ");
	print_num_hex(&id);
	printf(" will run... PC=0x");
	*/
	//id=e->env_tf.cp0_epc;
	/*print_num_hex(&id);
	printf("\n");

	printf("Recover New Env Regs:\n");
	*/	
	env_pop_tf(&(e->env_tf));  // �ָ���Ҫ���мĴ���ֵ
}



void env_idle()   // ϵͳ�ڵĿ��н���
{
	int i,j;

	while (1)
	{
	   printf("idle...");
	   //for (i=0;i<10000;i++);
	}
}

void env_idle_init()
{
	struct Env *e;

	e=&envs[0];

	curenv=e;

	e->env_id = 0;
	e->env_status = ENV_RUNNABLE;

	e->env_tf.pc=&env_idle-0x4;
    e->env_tf.cp0_epc= &env_idle-0x4;
	printf("Idle epc=0x");   //%x\n",e->env_tf.cp0_epc);
	
}

void env_idle_run()
{

    env_idle_init();

	printf("Idle Process 0 will running......\n");
	
	env_idle();
}


//////////////////////////////////////////////////////////////////////////
///////////   ����Ϊд��ϵͳ�ڵ��û����򣬴���Ϊ�˷��� //////////////
// �����õ�lib.h �� syscall_all.c �ﶨ��� �û�ϵͳ���ú���
// �� syscall_putchar��syscall_exit





void clear_reg(struct Env *e)
{
   int i;
   for (i=0;i<32;i++)
   {
      if (i!=29)
	     e->env_tf.regs[i]=0;
	}
}



unsigned int read_sr(void)
{
    unsigned int p ;
    
    asm( "mfc0 %0, $12" 
        :"=r"(p)
    ) ;
    
    return p ;
}

unsigned int read_cause(void)
{
    unsigned int p ;
    
    asm( "mfc0 %0, $13" 
        :"=r"(p)
    ) ;
    
    return p ;
}



void env_A()  // �û�����A
{
  int i;
  while (1)
  {
//	 printf("A");
  i = 65	 ;
  uput(i);
	 for (i=0;i<10;i++) ;
  }
  
 }

void env_B()  // // �û�����B
{
  int i;
  unsigned int sr,cause;
  while (1)
  {
  //   printf("B");
  i = 66;
  uput(i);
	 for (i=0;i<10;i++) ; 
  }
  
 }
 
 
void env_C() // // �û�����C
{
  int i;
  unsigned int sr,cause;

  while (1)
  {
  //   printf("C");
	i = 67;
  uput(i);
	for (i=0;i<10;i++) ; 
  }
  
 }
 
 
 void create_A() // �����û�����A
 {
 	struct Env *e;


	if (!(e = LIST_FIRST(&env_free_list))) 
		return -1;

	e->env_id = mkenvid(e);
	e->env_status = ENV_RUNNABLE;

	clear_reg(e);
	e->env_tf.regs[29] = 0x90720000;  // ��Ҫ������ö����spֵ����cp0_statusֵ
	e->env_tf.cp0_status = 0x00402001;
	
	e->env_tf.pc=&env_A;
    e->env_tf.cp0_epc=&env_A;  // �趨��A���̵���ʼ���е�ַ
 
	LIST_REMOVE(e, env_link);
 
 }
 
  void create_B()  // // �����û�����B
 {
 	struct Env *e;


	if (!(e = LIST_FIRST(&env_free_list))) 
		return -1;

	e->env_id = mkenvid(e);
	e->env_status = ENV_RUNNABLE;

	clear_reg(e);
	e->env_tf.regs[29] = 0x90730000; 
	e->env_tf.cp0_status = 0x00402001;
	
	e->env_tf.pc=&env_B;
    e->env_tf.cp0_epc=&env_B;
 
	LIST_REMOVE(e, env_link);
 
 }
 
  void create_C()   // �����û�����C
 {
 	struct Env *e;


	if (!(e = LIST_FIRST(&env_free_list))) 
		return -1;

	e->env_id = mkenvid(e);
	e->env_status = ENV_RUNNABLE;
	
	clear_reg(e);
	e->env_tf.regs[29] = 0x90740000; 
	e->env_tf.cp0_status = 0x00402001;
	
	e->env_tf.pc=&env_C;
    e->env_tf.cp0_epc=&env_C;
 
	LIST_REMOVE(e, env_link);
 
 }








	

	




