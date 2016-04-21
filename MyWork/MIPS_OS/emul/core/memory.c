/*
 * memory.c
 * by wuxb
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "memory.h"

unsigned int memory_size;		// = 0x08000000; // 128M

static void **memory = NULL;
static unsigned int piece_number;	// = 0x1000;  // 4K
static unsigned int write_mask_w;	// = 0x07FFFFFC;
static unsigned int write_mask_h;	// = 0x07FFFFFE;
static unsigned int write_mask_b;	// = 0x07FFFFFF;
static unsigned int piece_id_mask;	// = 0x07FF8000;

void
memory_init(unsigned int size)
{
	int mask;
	if (size & (size - 1)) {
		printf("What a fucking size!\n");
		return;
	}
	memory_free();
	memory_size = size;
	mask = size - 1;
	piece_number = size >> MEMORY_PIECE_ID_SHIFT;
	write_mask_w = mask & 0xFFFFFFFC;
	write_mask_h = mask & 0xFFFFFFFE;
	write_mask_b = mask;
	piece_id_mask = mask & MEMORY_PIECE_ID_MASK;

	memory = (void **) malloc((sizeof (void *)) * piece_number);
	memset(memory, 0, sizeof (void *) * piece_number);
}

void
memory_read_32(unsigned int address, unsigned int *data)
{
	int addr = address & write_mask_w;
	unsigned int piece;
	unsigned int offset;

	piece = (addr & piece_id_mask) >> MEMORY_PIECE_ID_SHIFT;

	if (!memory[piece]) {
		*data = 0;
	} else {
		offset = addr & MEMORY_PIECE_ADDR_MASK_W;
		offset >>= 2;
		*data = ((unsigned int *) (memory[piece]))[offset];
	}
}

void
memory_read_16(unsigned int address, unsigned short *data)
{
	int addr = address & write_mask_h;
	unsigned int piece;
	unsigned int offset;

	piece = (addr & piece_id_mask) >> MEMORY_PIECE_ID_SHIFT;
	if (!memory[piece]) {
		*data = 0;
	} else {
		offset = addr & MEMORY_PIECE_ADDR_MASK_H;
		offset >>= 1;
		*data = ((unsigned short *) (memory[piece]))[offset];
	}
}

void
memory_read_8(unsigned int address, unsigned char *data)
{
	int addr = address & write_mask_b;
	unsigned int piece;
	unsigned int offset;

	piece = (addr & piece_id_mask) >> MEMORY_PIECE_ID_SHIFT;

	if (!memory[piece]) {
		*data = 0;
	} else {
		offset = addr & MEMORY_PIECE_ADDR_MASK_B;
		*data = ((unsigned char *) (memory[piece]))[offset];
	}
}

void
memory_write_32(unsigned int address, unsigned int data)
{
	unsigned int addr = address & write_mask_w;
	unsigned int piece;
	unsigned int offset;

	piece = (addr & piece_id_mask) >> MEMORY_PIECE_ID_SHIFT;

	if (!memory[piece]) {
		memory[piece] = malloc(PIECE_SIZE);
		memset(memory[piece], 0, PIECE_SIZE);
	}

	offset = address & MEMORY_PIECE_ADDR_MASK_W;
	offset >>= 2;
	((unsigned int *) (memory[piece]))[offset] = data;
	return;
}

void
memory_write_16(unsigned int address, unsigned short data)
{
	unsigned int addr = address & write_mask_h;
	unsigned int piece;
	unsigned int offset;

	piece = (addr & piece_id_mask) >> MEMORY_PIECE_ID_SHIFT;

	if (!memory[piece]) {
		memory[piece] = malloc(PIECE_SIZE);
		memset(memory[piece], 0, PIECE_SIZE);
	}

	offset = addr & MEMORY_PIECE_ADDR_MASK_H;
	offset >>= 1;
	((unsigned short *) (memory[piece]))[offset] = data;
	return;
}

void
memory_write_8(unsigned int address, unsigned char data)
{
	unsigned int addr = address & write_mask_b;
	unsigned int piece;
	unsigned int offset;

	piece = (addr & piece_id_mask) >> MEMORY_PIECE_ID_SHIFT;

	if (!memory[piece]) {
		memory[piece] = malloc(PIECE_SIZE);
		memset(memory[piece], 0, PIECE_SIZE);
	}

	offset = addr & MEMORY_PIECE_ADDR_MASK_B;
	((unsigned char *) (memory[piece]))[offset] = data;
	return;
}

void
memory_free()
{
	int i;
	if (memory == NULL) {
		return;
	}
	for (i = 0; i < piece_number; i++) {
/*
free() frees the memory space pointed to by ptr, which must  have  been
       returned by a previous call to malloc(), calloc() or realloc().  Other-
       wise, or if free(ptr) has already been called before, undefined  behav-
       ior occurs.  If ptr is NULL, no operation is performed.
*/
		free(memory[i]);
	}
	free(memory);
	return;
}
