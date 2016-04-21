#include "include/lib.h"

int main()
{
     int i=0,j,n=5000;
 
     printf("Hello, User Env 2 is running...\n");
     while(++i<=n)
     {
          printf("B");
          if (i%50==0) putchar('\n');
  
     }
     printf("Bye, User Env 2 end running...\n");
	
     exit();

     return 0;
}
