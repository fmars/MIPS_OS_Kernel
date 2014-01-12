#include "include/printf.h"
#include "include/mm.h"
#include "user/A.char"
#include "user/B.char"
#include "user/C.char"
#include "include/env.h"

#include "include/arch_cocoa.h"

   
extern void create_A();
extern void create_B();
extern void create_C();

extern struct Env *curenv ;
extern int SCHED_N;

extern void except_vec3(); 
extern void env_idle_run();

unsigned char *pds_str="Hello,World! The System is starting...";

extern void printcharc(unsigned char ch);

int Next_EnvNum;

extern unsigned int read_sr(void);
extern unsigned int read_cause(void);

void print_num_hex(unsigned int *n);



/*
现在能简单轮流调度A、B、C三个进程， 系统调用代码用有，还没调通。
异常入口定义在interupt.S的exc_entry, 只有syscall的异常处理
中断入口定义在interupt.S的int_entry, 只有时钟引起中断的进程调度处理。
进程调度主要在寄存器的保存与恢复： 先保存原先运行进程寄存器上下文信息到0x90800000
以下38*4字节的位置，在env_run（）中再从此处保存到原先运行进程的进程控制块即Env结构中,
最后用env_pop_tf恢复即将运行程序的上下文，用eret指令跳到新进程原来被中断的位置继续运行。

问题：虽然三进程能轮流跑，但稳定性不好，调试很久不知原因所在。
即在用户进程A中， 每输出“A...” ， 都延迟一会儿，为了使输出慢些，能看清楚。
  while (1)
  {
     printf("A...");
	 for (i=0;i<1000;i++) ; 
  }
  但将 for (i=0;i<1000;i++) ; 改为 for (i=0;i<100;i++) ; 即使输出快些，
  则运行不正确，不能交替打印A、B、C，甚至停止输出。
  此时会发现SR寄存器EXL异常位被置1, 由于异常与中断对EPC操作不一样，即异常时EPC需加4,
  可能有问题。
  另外，也可能OS保存寄存器与恢复寄存器出现问题。
  还有， 也有可能是printcharc向串口输出字符时，由于没有原子性，可能被打断，可能出现异常；
  因为当用户程序不能正常运行时，EPC值大多为printcharc里某指令的地址。
*/

int main()
{
	unsigned int *p_led ;
	unsigned int *p_uart ;
	
	unsigned int i,j;
    	unsigned int sr,cause;
	/*fmars test operator ++
	int a[3]={3,5,4};
	int *b;
	b = a;
	uput(*b);
	b++;
	b++;
	uput(*b);
	
	//fmars test printf
	int t = 0;
	uput(t);
	uput(t+1);
	printf("a");
	printf("b");
	printf("a");
	i = sizeof(char);
	j = sizeof(unsigned char);
	uput(i);
	uput(j);
	uput(0);
*/

    p_led   = (unsigned int *)(BASE_DEV + MINOR_BASE_LED) ;
    p_uart  = (unsigned int *)(BASE_DEV + MINOR_BASE_UART0) ;

	*(p_uart + UART_DIV_SND/4) = 0xD9 ;  // 为串口UART0设定波特率
	
	Next_EnvNum=0;
	

	printf("Hello,World! The System is starting...\n");

	trap_init();
	env_init();	

	curenv = NULL;

	//ENV_CREATE(A); // 用户程序A、B、C, 方式一: 通过在user/src目录下编写用户程序，编译生成elf文件，
	//ENV_CREATE(B); // 用数组A.char、B.char、C.char编译进操作系统内
	//ENV_CREATE(C);
	
	printf("Begin to create process...\n");
	create_A(); // 用户程序A、B、C， 方式二： 定义在env.c中, 即直接在OS内编写用户程序
	create_B();
	create_C();
	printf("End to create process...\n");

	printf("Begin to start timer...\n");
	start_timer();  // 启动中断计时器

/*
	sr=read_sr();
	cause=read_cause();
	printf("SR=0x");
	print_num_hex(&sr);
	printf("  CAUSE=0x");
	print_num_hex(&cause);
	printf("\n");
*/
	
	printf("End to start timer...\n");

	env_idle_run();  // 空闲进程
	

	panic("^^^^^^^^^^^ This is the never reached end ^^^^^^^^^^^^^^\n");
	
	return 0;
}



void bcopy(const void *src, void *dst, size_t len)
{
	void *max;
	
	max = dst + len;
	if (((int)src%4==0)&&((int)dst%4==0))
	{
		while (dst + 3 < max)
		{
			*(int *)dst = *(int *)src;
			dst+=4;
			src+=4;
		}
	}
	
	while (dst < max)
	{
		*(char *)dst = *(char *)src;
		dst+=1;
		src+=1;
	}
}


void bzero(void *v, u_int n)
{
	char *p;
	int m;

	p = v;
	m = n;
	while (--m >= 0)
		*p++ = 0;
}





static unsigned int __inline__ myreadsr(void)
{
    unsigned int p ;
    
    asm( "mfc0 %0, $12" 
        :"=r"(p)
    ) ;
    
    return p ;
}

static unsigned int __inline__ myreadcause(void)
{
    unsigned int p ;
    
    asm( "mfc0 %0, $13" 
        :"=r"(p)
    ) ;
    
    return p ;
}


void  signal_exception( void ) // 调试用，表明进入异常处理 
{
    unsigned int sr, cause ;

    unsigned int *p_led ;


    p_led   = (unsigned int *)(BASE_DEV + MINOR_BASE_LED) ;
 

    cause = myreadcause() ;
    sr = myreadsr() ;
	
	*p_led=CAUSE_EXCCODE(cause)+128; 
    
//	printf("Enter exception...");


}


void  signal_int( void )  // 调试用，表明进入中断处理 
{
/*
    unsigned int sr, cause ;

    unsigned int *p_led ;
 
	

    p_led   = (unsigned int *)(BASE_DEV + MINOR_BASE_LED) ;
 

    cause = myreadcause() ;
    sr = myreadsr() ;
    
    // is interrupt 7?
    if ( IS_HWINT_7(cause, sr) )
        {*p_led = 7 ;}
    // is interrupt 6?
    if ( IS_HWINT_6(cause, sr) )
        {*p_led = 6 ;}
    // is interrupt 5?
    if ( IS_HWINT_5(cause, sr) ) {
        *p_led = 5 ;
    }
    // is interrupt 4?
    if ( IS_HWINT_4(cause, sr) )
        {*p_led = 4 ;}
    // is interrupt 3?
    if ( IS_HWINT_3(cause, sr) ) {
        *p_led = 3 ;
    }
    // is interrupt 2?
    if ( IS_HWINT_2(cause, sr) )
        {*p_led = 2 ;}
		
//fmars	
	printf("\nEnter Interruption............\n"); 
//	printf("Save Old Env Regs:\n");
*/
}


void  leave_int( void )
{
    //printf("Leave interrupt process program............\n\n");
}



void print_hex(unsigned char n)
{
     if (n<10) printcharc('0'+n);
	 else printcharc('A'+n-10);
}

void print_num_hex(unsigned int *n) // 输出整数16进制值
{
	   int i;
       unsigned char buf[50],*ch;
       unsigned char c,c_h,c_l;	   

	   ch=(unsigned int *)n;
	   ch=ch+3;
	   
	   for (i=0; i<4; i++ )
	   {
	      c=*ch--;
		  c_l=c & 0x0F;
		  c_h=c & 0xF0;
		  c_h=c_h>>4;
		  print_hex(c_h);
		  print_hex(c_l); 
	   }
		
}

void print_num_hex2(unsigned int *n)
{
	   int i;
       unsigned char buf[50],*ch;
       unsigned char c,c_h,c_l;	   

	   ch=(unsigned int *)n;
	   ch=ch+3;
	   
	   for (i=0; i<4; i++ )
	   {
	      c=*ch--;
		  c_l=c & 0x0F;
		  c_h=c & 0xF0;
		  c_h=c_h>>4;
		  if (c_h==0 && c_l==0 && i!=3) continue;
		  print_hex(c_h);
		  print_hex(c_l); 
	   }
		
}

void print_reg(unsigned int *n, int len)  // 调试用，打印寄存器值
{
    int i;
//fmars
	for (i=0;i<len;i++)
	{
	/*
	   if (i<32)
	   {
	        printf("$");
			print_num_hex2(&i);
	   }
	   else if (i==32)
	      printf("SR");
	   else if (i==33)
	      printf("HI");
	   else if (i==34)
	      printf("LO");
	   else if (i==35)
	      printf("CAUSE");
	   else if (i==36)
	      printf("EPC");
	   else if (i==37)
	      printf("PC");
	   
	   
	   printf("=0x");
	   print_num_hex(n);
	   printf("  ");
	   
	   if (i==7 || i==15 || i==23 || i==31) printf("\n"); 
		*/
	   n=n+1;
	}
	//printf("\n");  
}	   	  






