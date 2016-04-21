#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *fp_in;
FILE *fp_out;

int main(int argc, char **argv)
{
   int i,n;
   unsigned char ch;
   char elf[30];
   char out[30]="../";
   char s[30];

   if (argc<2)
   {
      printf("Please give the elf file name: name.elf\n");
      return -1;
   }

   i=0;
   strcpy(elf,argv[1]);
   while (elf[i]!='\0' && elf[i]!='.') i++;
   strncat(out,elf,i);
   strncpy(s,elf,i);
   strcat(out,".char");

   fp_in=fopen(elf,"r");
   fp_out=fopen(out,"w");

   n=0;
   fprintf(fp_out,"unsigned char binary_user_%s_start[]={\n",s);
   while (!feof(fp_in))
   {
      n++;
      ch=fgetc(fp_in);
      fprintf(fp_out,"0x%x",ch);
      if (ch<16) fprintf(fp_out," ");
      if (feof(fp_in))
      {
         fprintf(fp_out,"};\n");
      }
      else
      {
         fprintf(fp_out,", ");
         if (n%16==0)
            fprintf(fp_out,"\n");
      }
   }
   fprintf(fp_out,"unsigned int binary_user_%s_size=0x%x;\n",s,n);
   fclose(fp_in);
   fclose(fp_out);

}
   
      
       

    
