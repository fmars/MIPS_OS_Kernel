#include "include/dev_cons.h"
#include "include/env.h"
#include "include/print.h"
#include "include/printf.h"

extern char * KERNEL_SP;
extern struct Env * curenv;

#include "include/lib.h"

// 定义系统调用函数

void sys_putchar(char ch)
{
	printf("%c",ch);
}

void sys_exit()
{
	printf("exit\n");
}

void sys_printf()
{
	printf("printf\n");
}

