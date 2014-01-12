#ifndef _ELF_H__
#define _ELF_H__

#include "type.h"
/*
 * ELF header
 */

struct _ehdr_t 
{
    unsigned char ident[16];
    L4_Word16_t type;
    L4_Word16_t machine;
    L4_Word32_t version;
    L4_Word_t   entry;          // Program start address
    L4_Word_t   phoff;          // File offset of program header table
    L4_Word_t   shoff;
    L4_Word32_t flags;
    L4_Word16_t ehsize;         // Size of this ELF header
    L4_Word16_t phentsize;      // Size of a program header
    L4_Word16_t phnum;          // Number of program headers
    L4_Word16_t shentsize;
    L4_Word16_t shnum;
    L4_Word16_t shstrndx;
};

typedef struct _ehdr_t ehdr_t;

/*
 * Program header
 */

struct _phdr_t
{
    L4_Word32_t type;
    L4_Word_t   offset;
    L4_Word_t   vaddr;
    L4_Word_t   paddr;
    L4_Word_t   fsize;
    L4_Word_t   msize;
    L4_Word32_t	flags;
    L4_Word_t   align;
};

typedef struct _phdr_t phdr_t;

enum phdr_type_e 
{
    PT_LOAD =   1       /* Loadable program segment */
};

enum phdr_flags_e
{
    PF_X = 	1,
    PF_W = 	2,
    PF_R = 	4
};


/*
 * Section header
 */

struct shdr_t
{

    L4_Word32_t	name;
    L4_Word32_t	type;
    L4_Word_t	flags;
    L4_Word_t	addr;
    L4_Word_t	offset;
    L4_Word_t	size;
    L4_Word32_t	link;
    L4_Word32_t	info;
    L4_Word_t	addralign;

L4_Word_t	entsize;
};

enum shdr_type_e
{
    SHT_PROGBITS =	1,
    SHT_NOBITS =	8
};

enum shdr_flags_e
{
    SHF_WRITE =		1,
    SHF_ALLOC =		2,
    SHF_EXECINSTR =	4
}; 

#endif
