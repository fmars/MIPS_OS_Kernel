#ifndef _LIB_H__
#define _LIB_H__

#include "type.h"
#include "image.char"
#include <stdio.h>

static char MM[0x1000000];

char * des_trans(void *des)
{
	int d = (int)des;
	if ((d<0xBF000000)||(d>0xC0000000))
	{
		printf("des address err! 0x%x\n",d);
		exit(0);
	}

	return &(MM[d-0xBF000000]);
}

void *memcopy(void *destaddr,void const *srcaddr,int len)
{
	char *dest = des_trans(destaddr);
	char const *src = srcaddr;

	while(len-->0)
		*dest++=*src++;
	return destaddr;
}

void bzero(void *v, int n)
{
	char *p;
	int m;

	p = des_trans(v);
	m = n;
	while (--m >= 0)
		*p++ = 0;
}

void cleanimg()
{
	int i=0;
	for(;i<0x1000000;i++) MM[i]=0;
}

void makeimg()
{
	FILE * output = fopen("./flash_img","w");
	fwrite(MM,sizeof(char),0x1000000,output);
}

#endif
