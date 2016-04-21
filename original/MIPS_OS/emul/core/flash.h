/*
 * flash.h
 * by wuxb
 */

#define FLASH_IMAGE_FILE "flash_image"

//#define FLASH_MASK       0x00FFFFFF
#define FLASH_MASK       flash_mask
#define FLASH_MASK_B     FLASH_MASK
#define FLASH_MASK_H     (FLASH_MASK & 0xFFFFFFFE)
#define FLASH_MASK_W     (FLASH_MASK & 0xFFFFFFFC)

void flash_load(char * image_path, int writeback);
void flash_unload(void);
unsigned int flash_ok(void);

void flash_read_32(unsigned int address, unsigned int * data);
void flash_read_16(unsigned int address, unsigned short * data);
void flash_read_8(unsigned int address, unsigned char * data);

void flash_write_32(unsigned int address, unsigned int data);
void flash_write_16(unsigned int address, unsigned short data);
void flash_write_8(unsigned int address, unsigned char data);



