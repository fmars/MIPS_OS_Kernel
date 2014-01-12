#include "include/lib.h"

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




