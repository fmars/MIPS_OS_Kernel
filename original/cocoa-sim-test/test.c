static int abc=123456;
int xxx = 65443;
#include "rawapi.h"
/*
void fakeuart_write(int value)
{
    __asm__ __volatile__( \
      "sb $0,0x300(
    )
}
*/

int
main(int argc, char ** argv)
{
	int t;
	unsigned long long n;
	unsigned int a[4]={192,442,692,942};
    unsigned long long rt;
	t=1;
	while(t--){
		n=1;
		n++;
		rt = (n<<1)*1000 + a[n&3];
	}
    //fakeuart_write(rt);
    uput(abc);
    uput(rt);
	return rt ^ 0;
}

