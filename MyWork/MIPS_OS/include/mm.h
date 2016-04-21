#ifndef _MM_H__
#define _MM_H__

#define SEC_NUM 512
#define KSEG0_START 0x80000000
#define SEC_SIZE 0x100000 // 1M



#include "types.h"

void bcopy(const void *src, void *dst, size_t len);
void bzero(void *v, u_int n);

extern int MOD(int a,int b);
extern int DIV(int a,int b);
#endif
