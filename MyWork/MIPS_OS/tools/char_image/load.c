#include "lib.h"
#include "elf.h"
#include "type.h"
#include <stdio.h>

#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)>(y)?(x):(y))
#define GET32(x) get_32((unsigned char*)& x)
#define GET16(x) get_16((unsigned char*)& x)

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//big endian to little endian 
 
  unsigned  int  get_32(unsigned  char   * cmd)
   {
     unsigned  int   ret;
     int  i;
     for  (ret  =   0 , i  =  4  -   1 ; i  >=   0 ;  -- i)
      {
        ret   =  ret  <<   8 ;
        ret  |=  cmd[3-i];       
      }    
      return  ret;
   }
 
  unsigned  short  get_16(unsigned  char   * cmd)
   {
     unsigned  short   ret;
     int  i;
     for  (ret  =   0 , i  =  2  -   1 ; i  >=   0 ;  -- i)
      {
        ret   =  ret  <<   8 ;
        ret  |=  cmd[1-i];       
      }    
      return  ret;
   }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int elf_load (L4_Word_t file_start,
	       L4_Word_t file_end,
	       L4_Word_t *memory_start,
	       L4_Word_t *memory_end,
	       L4_Word_t *entry)
{
    // Pointer to ELF file header
    ehdr_t* eh = (ehdr_t*) file_start;

    // Is it an ELF file? Check ELF magic
    if (!((eh->ident[0] == 0x7F) &&
          (eh->ident[1] == 'E') &&
          (eh->ident[2] == 'L') &&
          (eh->ident[3] == 'F')))
    {
        return 0;
    }

	return elf_load32 (file_start, file_end,
			   memory_start, memory_end, entry);
}


int elf_load32 (L4_Word_t file_start,
			   L4_Word_t file_end,
			   L4_Word_t *memory_start,
			   L4_Word_t *memory_end,
			   L4_Word_t *entry)
{
    // Pointer to ELF file header
    ehdr_t* eh = (ehdr_t*) file_start;

    // Is it an executable?
    if (GET16(eh->type) != 2)
    {
        // No. Bail out
        printf(" 0x%x No executable\n",GET16(eh->type));
        return 0;
    }

    // Is the address of the PHDR table valid?
    if (GET32(eh->phoff) == 0)
    {
        // No. Bail out
        printf("Wrong PHDR table offset\n");
        return 0;
    }

        
    printf("   => %p\n", (void *)(L4_Word_t)GET32(eh->entry));

    // Locals to find the enclosing memory range of the loaded file
    L4_Word_t max_addr =  0U;
    L4_Word_t min_addr = ~0U;

    // Walk the program header table

	L4_Word_t i = 0;
    for (; i < GET16(eh->phnum); i++)
    {
        // Locate the entry in the program header table
        phdr_t* ph = (phdr_t*)(L4_Word_t)
	    (file_start + GET32(eh->phoff) + GET16(eh->phentsize) * i);
        
        // Is it to be loaded?
        if (GET32(ph->type) == PT_LOAD)
        {
            printf("  (%p,%p)->%p\n",
                   (void *)(L4_Word_t) (file_start + GET32(ph->offset)),
		   (void *)(L4_Word_t) GET32(ph->fsize),
		   (void *)(L4_Word_t) GET32(ph->paddr));
            // Copy bytes from "file" to memory - load address  --------Code pds------------- 
            memcopy(GET32(ph->paddr), file_start + GET32(ph->offset), GET32(ph->fsize));
	    // Zero "non-file" bytes                           ---------Data pds-------------
	    bzero (GET32(ph->paddr) + GET32(ph->fsize), GET32(ph->msize) - GET32(ph->fsize));
            // Update min and max
            min_addr = min(min_addr, GET32(ph->paddr));
            max_addr = max(max_addr, GET32(ph->paddr) + max(GET32(ph->msize), GET32(ph->fsize)));
        }
        
    }

    // Write back the values into the caller-provided locations
    *memory_start = min_addr;
    *memory_end = max_addr;
    *entry = GET32(eh->entry);

    // Signal successful loading
    return 1;
}

int main()
{
	L4_Word_t a;
	L4_Word_t b;
	L4_Word_t c;

	cleanimg();

	elf_load((L4_Word_t)binary_user_image_start,(L4_Word_t)binary_user_image_start+binary_user_image_size,&a,&b,&c);

	makeimg();

	return 0;
}
