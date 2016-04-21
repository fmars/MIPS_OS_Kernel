/*
 * address.c
 * by wuxb
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "address.h"
#include "memory.h"
#include "flash.h"
#include "cp0.h"
#include "cpu.h"
#include "timer.h"
#include "icu.h"
#include "iolet.h"

/* switch of cpu endian */
extern int cpu_big_endian;
extern unsigned int memory_size;
extern unsigned int flash_size;

static void fake_output_8(unsigned char data);
static void fake_output_bits32(unsigned int bits);

unsigned int
address_read_32(unsigned int address, unsigned int *data)
{
	unsigned int addr = address & 0xFFFFFFFC;

	if (address & 3) {
		return CP0_CAUSE_EC_ADEL;
	}

	if (addr >= MEMORY_START && addr <= MEMORY_END) {
		memory_read_32(addr - MEMORY_START, data);
	} else if (addr >= FLASH_START && addr <= FLASH_END) {
		flash_read_32(addr - FLASH_START, data);
	} else if (addr >= INTCTRL_START && addr <= INTCTRL_END) {
		icu_read_32(addr - INTCTRL_START, data);
	} else if (addr >= TIMER_START && addr <= TIMER_END) {
		timer_read_32(addr - TIMER_START, data);
/*	} else if (addr >= CONFREG_START && addr <= CONFREG_END) {*/

	} else if (addr >= UART_START && addr <= UART_END) {

/*	} else if (addr >= RESV_START && addr <= RESV_END) {*/

	} else if (addr >= LED_START && addr <= LED_END) {
		
/*	} else if (addr >= SWITCH_START && addr <= SWITCH_END) {*/

	} else {
		return CP0_CAUSE_EC_ADEL;
	}

	if (cpu_big_endian) {
		*data = (((*data) & 0x000000ff) << 24) |
			(((*data) & 0x0000ff00) << 8) |
			(((*data) & 0x00ff0000) >> 8) | (((*data) & 0xff000000) >> 24);
	}
	return 0;
}

unsigned int
address_read_16(unsigned int address, unsigned short *data)
{
	unsigned int addr = address & 0xFFFFFFFE;

	if (address & 1) {
		return CP0_CAUSE_EC_ADEL;
	}

	if (addr >= MEMORY_START && addr <= MEMORY_END) {
		memory_read_16(addr - MEMORY_START, data);
	} else if (addr >= FLASH_START && addr <= FLASH_END) {
		flash_read_16(addr - FLASH_START, data);
	} else if (addr >= INTCTRL_START && addr <= INTCTRL_END) {
		icu_read_16(addr - INTCTRL_START, data);
	} else if (addr >= TIMER_START && addr <= TIMER_END) {
		timer_read_16(addr - TIMER_START, data);
/*	} else if (addr >= CONFREG_START && addr <= CONFREG_END) {*/

	} else if (addr >= UART_START && addr <= UART_END) {

/*	} else if (addr >= RESV_START && addr <= RESV_END) {*/

	} else if (addr >= LED_START && addr <= LED_END) {

/*	} else if (addr >= SWITCH_START && addr <= SWITCH_END) {*/

	} else {
		return CP0_CAUSE_EC_ADEL;
	}

	if (cpu_big_endian) {
		*data = (((*data) & 0x00ff) << 8) | (((*data) & 0xff00) >> 8);
	}
	return 0;
}

unsigned int
address_read_8(unsigned int address, unsigned char *data)
{
	unsigned int addr = address & 0xFFFFFFFF;

	if (addr >= MEMORY_START && addr <= MEMORY_END) {
		memory_read_8(addr - MEMORY_START, data);
	} else if (addr >= FLASH_START && addr <= FLASH_END) {
		flash_read_8(addr - FLASH_START, data);
	} else if (addr >= INTCTRL_START && addr <= INTCTRL_END) {
		icu_read_8(addr - INTCTRL_START, data);
	} else if (addr >= TIMER_START && addr <= TIMER_END) {
		timer_read_8(addr - TIMER_START, data);
	} else if (addr >= LED_START && addr <= LED_END) {
		/* FAKE input :
		 *
		 * I'm not on hardware, make input easy.
		 */
		//fflush(stdin);
		//fflush(stdout);
/*		printf("input a character and press 'ENTER' ::");*/
/*		*data = getchar();*/
/*		fflush(stdin);*/

		/* async input */
		/*iolet_read_8(addr - LED_START, data);*/
/*	} else if (addr >= CONFREG_START && addr <= CONFREG_END) {*/

	} else if (addr >= UART_START && addr <= UART_END) {
		/* here use fake console. */

/*	} else if (addr >= RESV_START && addr <= RESV_END) {*/

/*	} else if (addr >= SWITCH_START && addr <= SWITCH_END) {*/

	} else {
		return CP0_CAUSE_EC_ADEL;
	}
	return 0;
}

unsigned int
address_write_32(unsigned int address, unsigned int data)
{
	unsigned int addr = address & 0xFFFFFFFC;
	if (address & 3) {
		return CP0_CAUSE_EC_ADES;
	}

	if (cpu_big_endian) {
		data = ((data & 0x000000ff) << 24) |
			((data & 0x0000ff00) << 8) |
			((data & 0x00ff0000) >> 8) | ((data & 0xff000000) >> 24);
	}

	if (addr >= MEMORY_START && addr <= MEMORY_END) {
		memory_write_32(addr - MEMORY_START, data);
	} else if (addr >= FLASH_START && addr <= FLASH_END) {
		flash_write_32(addr - FLASH_START, data);
	} else if (addr >= INTCTRL_START && addr <= INTCTRL_END) {
		icu_write_32(addr - INTCTRL_START, data);
	} else if (addr >= TIMER_START && addr <= TIMER_END) {
		timer_write_32(addr - TIMER_START, data);
/*	} else if (addr >= CONFREG_START && addr <= CONFREG_END) {*/

	} else if (addr >= UART_START && addr <= UART_END) {

/*	} else if (addr >= RESV_START && addr <= RESV_END) {*/

	} else if (addr >= LED_START && addr <= LED_END) {
        fake_output_bits32(data);
		//write_led_32(addr, data);
		//printf(" %x(%u) ", data, data);
/*	} else if (addr >= SWITCH_START && addr <= SWITCH_END) {*/

	} else {
		return CP0_CAUSE_EC_ADES;
	}
	return 0;
}

unsigned int
address_write_16(unsigned int address, unsigned short data)
{
	unsigned int addr = address & 0xFFFFFFFE;

	if (address & 1) {
		return CP0_CAUSE_EC_ADES;
	}

	if (cpu_big_endian) {
		data = ((data & 0x00ff) << 8) | ((data & 0xff00) >> 8);
	}

	if (addr >= MEMORY_START && addr <= MEMORY_END) {
		memory_write_16(addr - MEMORY_START, data);
	} else if (addr >= FLASH_START && addr <= FLASH_END) {
		flash_write_16(addr - FLASH_START, data);
	} else if (addr >= INTCTRL_START && addr <= INTCTRL_END) {
		icu_write_16(addr - INTCTRL_START, data);
	} else if (addr >= TIMER_START && addr <= TIMER_END) {
		timer_write_16(addr - TIMER_START, data);
/*	} else if (addr >= CONFREG_START && addr <= CONFREG_END) {*/

	} else if (addr >= UART_START && addr <= UART_END) {

/*	} else if (addr >= RESV_START && addr <= RESV_END) {*/

	} else if (addr >= LED_START && addr <= LED_END) {
		//printf(" %x(%u) ", data, data);
/*	} else if (addr >= SWITCH_START && addr <= SWITCH_END) {*/

	} else {
		return CP0_CAUSE_EC_ADES;
	}
	return 0;
}

unsigned int
address_write_8(unsigned int address, unsigned char data)
{
	unsigned int addr = address;

	if (addr >= MEMORY_START && addr <= MEMORY_END) {
		memory_write_8(addr - MEMORY_START, data);
	} else if (addr >= FLASH_START && addr <= FLASH_END) {
		flash_write_8(addr - FLASH_START, data);
	} else if (addr >= INTCTRL_START && addr <= INTCTRL_END) {
		icu_write_8(addr - INTCTRL_START, data);
	} else if (addr >= TIMER_START && addr <= TIMER_END) {
		timer_write_8(addr - TIMER_START, data);
	} else if (addr >= LED_START && addr <= LED_END) {
		fake_output_8(data);
/*	} else if (addr >= CONFREG_START && addr <= CONFREG_END) {*/

	} else if (addr >= UART_START && addr <= UART_END) {

/*	} else if (addr >= RESV_START && addr <= RESV_END) {*/

/*	} else if (addr >= SWITCH_START && addr <= SWITCH_END) {*/

	} else {
		return CP0_CAUSE_EC_ADES;
	}
	return 0;
}

static void
fake_output_8(unsigned char data)
{
    printf("\n  --8bits output: [0x%02x]\n", data);
}

static void
fake_output_bits32(unsigned int bits)
{
/*
    if( (0x41 <= bits && bits <= 0x5a) || (0x61 <= bits && bits <= 0x7a) ||(0x30 <= bits && bits <= 0x39))
	output_str[ output_ptr++ ] = bits;
    	//printf("  --32bits output: [0x%08x]--> %c\n", bits, bits);
    else if (bits == '\n'){
	for(i = 0; i < output_ptr; i++)
		printf("%c",output_str[i]);
	printf("\n");
	output_ptr = 0;
	}
    else
    	printf("  --32bits output: [0x%08x]\n", bits);
    printf("  --32bits output: [0x%08x]--> %c\n", bits, bits);
*/
    printf("%c",bits);
}
