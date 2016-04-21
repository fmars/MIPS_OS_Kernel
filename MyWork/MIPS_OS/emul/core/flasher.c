/*
 * flasher.c
 * by wuxb
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>

#include "flasher.h"
#include "disass.h"

#define FLASHER_INFO \
"flash image maker, v0.1, wuxb, 2008-2-12\n\
Image address: BF00 0000 - BFFF FFFF\n\
command :\n\
\t q - quit.\n\
\t l - load data from file : l [hex address] [file name]\n\
\t w - write data by stdio : w [hex address] (0-9a-f)+\n\
\t v - view data of image  : v [hex address]\n\
\t h - help\n\
\t d - disassemble instruction : d [hex address]\n\
\t\t show 256 bytes.\n"

#define HELP_INFO_1 \
"Example of 'l'oad command:\n\
>> l 100 data.dat\n\
\t means : load data from file \"data.dat\", start address is 0x100.\n"

#define FLASHER_INPUT_SIGN "FLASHER >> "
#define DEFAULT_SIZE 0x01000000
#define DEFAULT_FILE "flash_image"

static void *image_mmap = NULL;
static unsigned int image_size = DEFAULT_SIZE;
static char *image_file = DEFAULT_FILE;

static void load_from_file(char *buffer);
static void view_data(char *buffer);
static void write_from_cmd(char *buffer);
static void dis_asm(char *buffer, int endian);

static void
load_from_file(char *buffer)
{
	unsigned int address;
	int lread;
	char file[128];
	//unsigned char data[64];
	void *mmap;
	FILE *input;
	unsigned int wcount = 0;

	address = image_size;
	sscanf(buffer, "%x %s", &address, file);

	input = fopen(file, "rb");
	if (!input) {
		printf("no this file\n");
		return;
	}
//  address &= FLASH_MASK;
	mmap = image_mmap + address;
	lread = 256;
	while (!feof(input)) {
		if (image_size - address < 256) {
			lread = image_size - address;
		}
		wcount += fread(mmap, 1, lread, input);
		address += lread;
		mmap += lread;
		if (address >= image_size) {
			break;
		}
	}
	printf("%d bytes has been copyed.\n", wcount);
	fclose(input);
}

static void
view_data(char *buffer)
{
	int address;
	//char data[256];
	unsigned char *pdata;
	int i, j;
	sscanf(buffer, "%x", &address);
	//address &= (FLASH_MASK & 0xFFFFFFF0);
	//fseek(output, address, SEEK_SET);
	//fread(data, 1, 256, output);
	//printf("#%d#\n", address);
	if (address + 256 >= image_size) {
		address = image_size - 0x100;
	}
	pdata = image_mmap + address;
	printf("BASE\\OFF + 00 01 02 03  04 05 06 07  08 09 0A 0B  0C 0D 0E 0F\n");
	for (i = 0; i < 16; i++) {
		printf("%08x | ", address + (i << 4));
		for (j = 0; j < 16; j++) {
			printf("%02x", (unsigned char) pdata[(i << 4) + j]);
			if (j == 3 || j == 7 || j == 11)
				printf("--");
			else
				printf(" ");
		}
		printf("\n");
	}
}

static void
write_from_cmd(char *buffer)
{
	unsigned int address = image_size;
	char *p;
	int wcount = 0;
	unsigned char data;
	unsigned char *pdata;
	if (sscanf(buffer, "%x", &address) == 0) {
		return;
	}

	pdata = (unsigned char *) (image_mmap + address);

	p = strchr(buffer, ' ') + 1;
	for (;;) {
		if (address >= image_size)
			break;
		data = 0;
		switch (*p) {
		case '0':
			break;
		case '1':
			data |= 0x10;
			break;
		case '2':
			data |= 0x20;
			break;
		case '3':
			data |= 0x30;
			break;
		case '4':
			data |= 0x40;
			break;
		case '5':
			data |= 0x50;
			break;
		case '6':
			data |= 0x60;
			break;
		case '7':
			data |= 0x70;
			break;
		case '8':
			data |= 0x80;
			break;
		case '9':
			data |= 0x90;
			break;
		case 'a':
		case 'A':
			data |= 0xa0;
			break;
		case 'b':
		case 'B':
			data |= 0xb0;
			break;
		case 'c':
		case 'C':
			data |= 0xc0;
			break;
		case 'd':
		case 'D':
			data |= 0xd0;
			break;
		case 'e':
		case 'E':
			data |= 0xe0;
			break;
		case 'f':
		case 'F':
			data |= 0xf0;
			break;
		default:
			printf(":) %d bytes is written.\n", wcount);
			return;
		}
		++p;

		switch (*p) {
		case '0':
			break;
		case '1':
			data |= 1;
			break;
		case '2':
			data |= 2;
			break;
		case '3':
			data |= 3;
			break;
		case '4':
			data |= 4;
			break;
		case '5':
			data |= 5;
			break;
		case '6':
			data |= 6;
			break;
		case '7':
			data |= 7;
			break;
		case '8':
			data |= 8;
			break;
		case '9':
			data |= 9;
			break;
		case 'a':
		case 'A':
			data |= 10;
			break;
		case 'b':
		case 'B':
			data |= 11;
			break;
		case 'c':
		case 'C':
			data |= 12;
			break;
		case 'd':
		case 'D':
			data |= 13;
			break;
		case 'e':
		case 'E':
			data |= 14;
			break;
		case 'f':
		case 'F':
			data |= 15;
			break;
		default:
			printf(":( %d bytes is written.\n", wcount);
			return;
		}
		++p;
		*pdata = data;
		pdata++;
		address++;
		//fwrite(&data, 1, 1, output);

		wcount++;
		while (*p == ' ') {
			if (*p == 0) {
				return;
			}
			p++;
		}
	}
	printf("%d bytes is written.\n", wcount);
}

static void
dis_asm(char *buffer, int endian)
{
	unsigned int address;
	unsigned int *inst;
	unsigned int i;
	unsigned char temp[4];

	int flag; //pds ----------------------

	if (sscanf(buffer, "%x", &address) < 1) {
		printf("bad address!\n");
		return;
	}

	address &= 0xFFFFFFFC;

	inst = (unsigned int *) (image_mmap + address);
	for (i = 0; i < 16; i++) {
		if (address >= image_size)
			return;

		if (endian) {
			temp[0] = ((unsigned char *) (&inst[i]))[3];
			temp[1] = ((unsigned char *) (&inst[i]))[2];
			temp[2] = ((unsigned char *) (&inst[i]))[1];
			temp[3] = ((unsigned char *) (&inst[i]))[0];
			printf("[%08x]:[%08x]", address, *((unsigned int *) temp));
			disassemble(*((unsigned int *) temp),&flag);
		} else {
			printf("[%08x]:[%08x]", address, inst[i]);
			disassemble(inst[i],&flag);
		}
		address += 4;
	}
}

int
main(int argc, char **argv)
{
	char buffer[1024] = { 0 };
/*	FILE *output;*/
	int i;
	int fd;
	int opt;
/*	output = fopen(FLASH_IMAGE_FILE, "rb+");
	if (!output) {
		printf("new image created\n");
		output = fopen(FLASH_IMAGE_FILE, "wb+");
		for (i = 0; i < 0x4000; i++) {
			fwrite(buffer, 1, 1024, output);
		}
	}*/

	while ((opt = getopt(argc, argv, "l:f:")) != -1) {
		switch (opt) {
		case 'l':
			sscanf(optarg, "%u", &image_size);
			break;
		case 'f':
			image_file = optarg;
			break;
		}
	}
	printf("FLASH IMAGE SIZE : %xB\n", image_size);
	printf("FLASH FILE : %s\n", image_file);

	if (access(image_file, F_OK) == -1) {
		fd = open(image_file, O_CREAT | O_RDWR, 00644);
		if (fd == -1) {
			printf("err\n");
			exit(1);
		}
		ftruncate(fd, image_size);
	} else {
		fd = open(image_file, O_RDWR);
		if (fd == -1) {
			printf("err\n");
			exit(1);
		}
		if (lseek(fd, 0, SEEK_END) != image_size) {
			ftruncate(fd, image_size);
		}
	}

	image_mmap =
		mmap(NULL, image_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (image_mmap == MAP_FAILED) {
		image_mmap = NULL;
		return 0;
	}
/*	if (argc > 1) {
		//use argv[1] to execute, then quit.
		strcpy(buffer, argv[2]);
		i = 3;
		while (i < argc) {
			strcat(buffer, " ");
			strcat(buffer, argv[i]);
			i++;
		}
		// buffer: ptr(input + 2).
		switch (argv[1][0]) {
		case 'l':
			load_from_file(buffer, output);
			break;

		case 'w':
			write_from_cmd(buffer, output);
			break;

		case 'v':
			view_data(buffer, output);
			break;

		case 'd':
			dis_asm(buffer, output);
			break;

		default:
			printf("sorry..\n");
			return 1;
		}
		return 0;
	}*/
	printf(FLASHER_INFO);
	printf(FLASHER_INPUT_SIGN);
	for (;;) {
		fgets(buffer, 1020, stdin);
		i = 0;
		while (buffer[i]) {
			if (buffer[i] == '\n') {
				buffer[i] = 0;
			}
			i++;
		}
		switch (buffer[0]) {
		case 'q':
			// msync(image_mmap, image_size, MS_ASYNC);
			munmap(image_mmap, image_size);
			printf("bye\n");
			return 0;

		case 'l':
			load_from_file(buffer + 2);
			break;

		case 'w':
			write_from_cmd(buffer + 2);
			break;

		case 'v':
			view_data(buffer + 2);
			break;

		case 'h':
			//puts(HELP_INFO_1);
			puts(FLASHER_INFO);
			break;

		case 'd':
			dis_asm(buffer + 2, 0);
			break;

		case 'D':
			dis_asm(buffer + 2, 1);
			break;

		default:
			printf("sorry..\n");
			return 1;

		}
		printf(FLASHER_INPUT_SIGN);
	}
}
