#include "include/dev_cons.h"
#include "include/env.h"
#include "include/print.h"
#include "include/printf.h"

extern char * KERNEL_SP;
extern struct Env * curenv;

#include "include/lib.h"

// ¶¨ÒåÏµÍ³µ÷ÓÃº¯Êý

void syscall_putchar(char ch)
{
	msyscall(SYS_putchar,(int)ch,0,0,0,0);
}

void syscall_printf(char *fmt)
{
	msyscall(SYS_printf,fmt,0,0,0,0);
}

void syscall_exit()
{
	msyscall(SYS_exit,0,0,0,0,0);
}


