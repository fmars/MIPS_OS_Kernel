
#include "include/env.h"
#include "include/printf.h"

extern void handle_int();
extern void handle_reserved();

extern void handle_sys();

unsigned long exception_handlers[32];

void trap_init(){  // 注册中断、异常处理函数
	int i;

	for(i=0;i<32;i++)
	    set_except_vector(i, handle_reserved);

	/* Here only register two exception */
	set_except_vector(0, handle_int); // ExcCode=0 interruption Exception
	set_except_vector(8, handle_sys); // ExcCode=8 syscall      Exception
}

void *set_except_vector(int n, void * addr){
	unsigned long handler=(unsigned long)addr;
	unsigned long old_handler=exception_handlers[n];

	exception_handlers[n]=handler;
	return (void *)old_handler;
}


