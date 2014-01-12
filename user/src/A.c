#include "include/lib.h"

int main()
{
	char s[]="pds:Hello World!\n";
	char t[]="This is a string!";
	int i=0,j,n=17;

	while(1)
	{

	printf("\n\nThis is Env 1......\n");
	printf("%s %d\n",t,n);

	i=0;
	while (i<n)
	    putchar(s[i++]);

	for (i=0; i<30; i++)
	{
	   for (j=0;j<50;j++)
              putchar('A');
	    putchar('\n');
	}
	}

	printf("This is the end of User Env 1...\n");

	exit();
	

	return 0;


 /*      
     printf("Hello, User Env 1 is running...\n");
     while(++i<=n)
     {
          printf("A");
          if (i%100==0) printf(" ");
          for (j=0;j<10;j++)
              for (k=0;k<10;k++)
                      ;
     }
     printf("Bye, User Env 1 end running...\n");
*/
}
