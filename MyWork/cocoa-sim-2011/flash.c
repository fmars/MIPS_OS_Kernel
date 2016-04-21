/*
 * flash.c
 * by wuxb
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "flash.h"
unsigned int flash_size;

static void *f_mmap = NULL;

void
flash_load(char *image_path, int writeback)
{
	int fd;
	/* image has opened, just sync it or even not do anything. */
	if (f_mmap) {
		msync(f_mmap, flash_size, MS_ASYNC);
		return;
	}
	if (image_path) {
		fd = open(image_path, O_RDWR);
	} else {
		fd = open(FLASH_IMAGE_FILE, O_RDWR);
	}
	if (fd == -1)
		return;

	flash_size = (unsigned int) lseek(fd, 0, SEEK_END);
	f_mmap = mmap(NULL, flash_size, PROT_READ | PROT_WRITE, 
			(writeback ? MAP_SHARED : MAP_PRIVATE), fd, 0);
	if (f_mmap == MAP_FAILED) {
		f_mmap = NULL;
	}
	close(fd);
}

void
flash_unload()
{
	munmap(f_mmap, flash_size);
	f_mmap = NULL;
}

/* test if flash image is available. */
unsigned int
flash_ok()
{
	if (f_mmap != (void *) (-1))
		return 1;
	else
		return 0;
}

void
flash_read_32(unsigned int address, unsigned int *data)
{
	if (address >= flash_size || address & 3)
		return;

	*data = *(((unsigned int *) f_mmap) + (address >> 2));
}

void
flash_read_16(unsigned int address, unsigned short *data)
{
	if (address >= flash_size || address & 1)
		return;
	*data = *(((unsigned short *) f_mmap) + (address >> 1));
}

void
flash_read_8(unsigned int address, unsigned char *data)
{
	if (address >= flash_size)
		return;
	*data = *(((unsigned char *) f_mmap) + address);
}

void
flash_write_32(unsigned int address, unsigned int data)
{
	if (address >= flash_size || address & 3)
		return;

	*(((unsigned int *) f_mmap) + (address >> 2)) = data;
}

void
flash_write_16(unsigned int address, unsigned short data)
{
	if (address >= flash_size || address & 1)
		return;
	*(((unsigned short *) f_mmap) + (address >> 1)) = data;
}

void
flash_write_8(unsigned int address, unsigned char data)
{
	if (address >= flash_size)
		return;
	*(((unsigned char *) f_mmap) + address) = data;
}
