/*
 * memory.h
 * by wuxb
 */
/* size of memory must be 2^n */

//32kb
#define PIECE_SIZE    0x8000
//4kb
//#define PIECE_NUMBER  0x1000
#define PIECE_NUMBER   (piece_number)

//#define MEMORY_ADDRESS_MASK_W     0xFFFFFFFC
//#define MEMORY_ADDRESS_MASK_H     0xFFFFFFFE
//#define MEMORY_ADDRESS_MASK_B     0xFFFFFFFF

#define MEMORY_PIECE_ID_MASK      0xFFFF8000

#define MEMORY_PIECE_ID_SHIFT     15
#define MEMORY_PIECE_ADDR_MASK_W  0x00007FFC
#define MEMORY_PIECE_ADDR_MASK_H  0x00007FFE
#define MEMORY_PIECE_ADDR_MASK_B  0x00007FFF

void memory_init(unsigned int size);
void memory_read_32(unsigned int address, unsigned int * data);
void memory_read_16(unsigned int address, unsigned short * data);
void memory_read_8(unsigned int address, unsigned char * data);
void memory_write_32(unsigned int address, unsigned int data);
void memory_write_16(unsigned int address, unsigned short data);
void memory_write_8(unsigned int address, unsigned char data);
void memory_free(void);
