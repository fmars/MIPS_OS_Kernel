#include "include/lib.h"

int main()
{
     int i=0,n=10000;
 
     printf("Hello, User Env 3 is running...\n");
     while(++i<=n)
     {
          printf("C");
          if (i%50==0) putchar('\n');
     }
     printf("Bye, User Env 3 end running...\n");
	
     exit();

     return 0;
}
