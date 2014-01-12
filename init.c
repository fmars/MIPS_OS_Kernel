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
�����ܼ���������A��B��C�������̣� ϵͳ���ô������У���û��ͨ��
�쳣��ڶ�����interupt.S��exc_entry, ֻ��syscall���쳣����
�ж���ڶ�����interupt.S��int_entry, ֻ��ʱ�������жϵĽ��̵��ȴ���
���̵�����Ҫ�ڼĴ����ı�����ָ��� �ȱ���ԭ�����н��̼Ĵ�����������Ϣ��0x90800000
����38*4�ֽڵ�λ�ã���env_run�������ٴӴ˴����浽ԭ�����н��̵Ľ��̿��ƿ鼴Env�ṹ��,
�����env_pop_tf�ָ��������г���������ģ���eretָ�������½���ԭ�����жϵ�λ�ü������С�

���⣺��Ȼ�������������ܣ����ȶ��Բ��ã����Ժܾò�֪ԭ�����ڡ�
�����û�����A�У� ÿ�����A...�� �� ���ӳ�һ�����Ϊ��ʹ�����Щ���ܿ������
  while (1)
  {
     printf("A...");
	 for (i=0;i<1000;i++) ; 
  }
  ���� for (i=0;i<1000;i++) ; ��Ϊ for (i=0;i<100;i++) ; ��ʹ�����Щ��
  �����в���ȷ�����ܽ����ӡA��B��C������ֹͣ�����
  ��ʱ�ᷢ��SR�Ĵ���EXL�쳣λ����1, �����쳣���ж϶�EPC������һ�������쳣ʱEPC���4,
  ���������⡣
  ���⣬Ҳ����OS����Ĵ�����ָ��Ĵ����������⡣
  ���У� Ҳ�п�����printcharc�򴮿�����ַ�ʱ������û��ԭ���ԣ����ܱ���ϣ����ܳ����쳣��
  ��Ϊ���û���������������ʱ��EPCֵ���Ϊprintcharc��ĳָ��ĵ�ַ��
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

	*(p_uart + UART_DIV_SND/4) = 0xD9 ;  // Ϊ����UART0�趨������
	
	Next_EnvNum=0;
	

	printf("Hello,World! The System is starting...\n");

	trap_init();
	env_init();	

	curenv = NULL;

	//ENV_CREATE(A); // �û�����A��B��C, ��ʽһ: ͨ����user/srcĿ¼�±�д�û����򣬱�������elf�ļ���
	//ENV_CREATE(B); // ������A.char��B.char��C.char���������ϵͳ��
	//ENV_CREATE(C);
	
	printf("Begin to create process...\n");
	create_A(); // �û�����A��B��C�� ��ʽ���� ������env.c��, ��ֱ����OS�ڱ�д�û�����
	create_B();
	create_C();
	printf("End to create process...\n");

	printf("Begin to start timer...\n");
	start_timer();  // �����жϼ�ʱ��

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

	env_idle_run();  // ���н���
	

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


void  signal_exception( void ) // �����ã����������쳣���� 
{
    unsigned int sr, cause ;

    unsigned int *p_led ;


    p_led   = (unsigned int *)(BASE_DEV + MINOR_BASE_LED) ;
 

    cause = myreadcause() ;
    sr = myreadsr() ;
	
	*p_led=CAUSE_EXCCODE(cause)+128; 
    
//	printf("Enter exception...");


}


void  signal_int( void )  // �����ã����������жϴ��� 
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

void print_num_hex(unsigned int *n) // �������16����ֵ
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

void print_reg(unsigned int *n, int len)  // �����ã���ӡ�Ĵ���ֵ
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






