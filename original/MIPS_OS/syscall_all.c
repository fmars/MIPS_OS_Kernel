#include "include/dev_cons.h"
#include "include/env.h"
#include "include/print.h"
#include "include/printf.h"

extern char * KERNEL_SP;
extern struct Env * curenv;

#include "include/lib.h"

// 定义系统调用函数

void syscall_putchar(char ch)
{
	pds_syscall(SYS_putchar,(int)ch,0,0,0,0);
}

void syscall_printf(char *fmt)
{
	pds_syscall(SYS_printf,fmt,0,0,0,0);
}

void syscall_exit()
{
	pds_syscall(SYS_exit,0,0,0,0,0);
}


