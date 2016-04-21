/*
 * uart.c, by wuxb
 * also fake...
 */
#include<string.h>

#include"uart.h"

static struct RUART uartReg;

void
uart_reset()
{
	memset(&uartReg, 0, sizeof (struct RUART));
	uartReg.lsr &= UART_LSR_THRE_MASK;
	uartReg.divr = 0xa2;
	uartReg.divs = 0xa2;
}

void
uart_read_32(unsigned int address, unsigned int *data)
{
	unsigned int addr = address & 0x000000FC;
	unsigned int data32;

	switch (addr) {
	case UART_OFFSET_DATA:
		data32 = (uartReg.data & UART_DATA_MASK);
		break;
	case UART_OFFSET_IER:
		data32 = (uartReg.ier & UART_IER_MASK);
		break;
	case UART_OFFSET_IIR:
		data32 = (uartReg.iir & UART_IIR_MASK);
		break;
	case UART_OFFSET_LCR:
		data32 = (uartReg.lcr & UART_LCR_MASK);
		break;
	case UART_OFFSET_LSR:
		data32 = (uartReg.lsr & UART_LSR_MASK);
		break;
	case UART_OFFSET_DIVR:
		data32 = (uartReg.divr & UART_DIVR_MASK);
		break;
	case UART_OFFSET_DIVS:
		data32 = (uartReg.divs & UART_DIVS_MASK);
		break;
	default:
		data32 = 0;
		break;
	}
	*data = data32;
}

void
uart_read_16(unsigned int address, unsigned short *data)
{
	unsigned int addr = address & 0x000000FC;
	unsigned int data32;

	switch (addr) {
	case UART_OFFSET_DATA:
		data32 = (uartReg.data & UART_DATA_MASK);
		break;
	case UART_OFFSET_IER:
		data32 = (uartReg.ier & UART_IER_MASK);
		break;
	case UART_OFFSET_IIR:
		data32 = (uartReg.iir & UART_IIR_MASK);
		break;
	case UART_OFFSET_LCR:
		data32 = (uartReg.lcr & UART_LCR_MASK);
		break;
	case UART_OFFSET_LSR:
		data32 = (uartReg.lsr & UART_LSR_MASK);
		break;
	case UART_OFFSET_DIVR:
		data32 = (uartReg.divr & UART_DIVR_MASK);
		break;
	case UART_OFFSET_DIVS:
		data32 = (uartReg.divs & UART_DIVS_MASK);
		break;
	default:
		data32 = 0;
		break;
	}

	if (address & 2) {
		*data = (unsigned short) (data32 >> 16);
	} else {
		*data = (unsigned short) data32;
	}
}

void
uart_read_8(unsigned int address, unsigned char *data)
{
	unsigned int addr = address & 0x000000FC;
	unsigned int data32;

	switch (addr) {
	case UART_OFFSET_DATA:
		data32 = (uartReg.data & UART_DATA_MASK);
		break;
	case UART_OFFSET_IER:
		data32 = (uartReg.ier & UART_IER_MASK);
		break;
	case UART_OFFSET_IIR:
		data32 = (uartReg.iir & UART_IIR_MASK);
		break;
	case UART_OFFSET_LCR:
		data32 = (uartReg.lcr & UART_LCR_MASK);
		break;
	case UART_OFFSET_LSR:
		data32 = (uartReg.lsr & UART_LSR_MASK);
		break;
	case UART_OFFSET_DIVR:
		data32 = (uartReg.divr & UART_DIVR_MASK);
		break;
	case UART_OFFSET_DIVS:
		data32 = (uartReg.divs & UART_DIVS_MASK);
		break;
	default:
		data32 = 0;
		break;
	}

	switch (address & 3) {
	case 0:
		*data = (unsigned char) data32;
		break;
	case 1:
		*data = (unsigned char) (data32 >> 8);
		break;
	case 2:
		*data = (unsigned char) (data32 >> 16);
		break;
	case 3:
		*data = (unsigned char) (data32 >> 24);
		break;
	}
}

void
uart_write_32(unsigned int address, unsigned int data)
{
	unsigned int addr = address & 0x000000FC;
	unsigned int data32;

	data32 = data;

	switch (addr) {
	case UART_OFFSET_DATA:
		uartReg.data = data32 & UART_DATA_MASK;

		break;
	case UART_OFFSET_IER:
		uartReg.ier = data32 & UART_IER_MASK;

		break;
	case UART_OFFSET_IIR:
		/* read only. */

		break;
	case UART_OFFSET_LCR:
		uartReg.lcr = data32 & UART_LCR_MASK;

		break;
	case UART_OFFSET_LSR:
		/* read only. */

		break;
	case UART_OFFSET_DIVR:
		uartReg.divr = data32 & UART_DIVR_MASK;

		break;
	case UART_OFFSET_DIVS:
		uartReg.divs = data32 & UART_DIVR_MASK;

		break;
	default:
		break;
	}

}

void
uart_write_16(unsigned int address, unsigned short data)
{
	unsigned int addr = address & 0x000000FC;
	unsigned int data32;
	unsigned int mask;

	if (address & 2) {
		data32 = ((unsigned int) data) << 16;
		mask = 0xFFFF0000;
	} else {
		data32 = (unsigned int) data;
		mask = 0x0000FFFF;
	}

	switch (addr) {
	case UART_OFFSET_DATA:
		uartReg.data =
			((uartReg.data & (~mask)) | (data32 & mask)) & UART_DATA_MASK;
		break;
	case UART_OFFSET_IER:
		uartReg.ier =
			((uartReg.ier & (~mask)) | (data32 & mask)) & UART_IER_MASK;
		break;
	case UART_OFFSET_IIR:
		/* read only. */

		break;
	case UART_OFFSET_LCR:
		uartReg.lcr =
			((uartReg.lcr & (~mask)) | (data32 & mask)) & UART_LCR_MASK;
		break;
	case UART_OFFSET_LSR:
		/* read only. */

		break;
	case UART_OFFSET_DIVR:
		uartReg.divr =
			((uartReg.divr & (~mask)) | (data32 & mask)) & UART_DIVR_MASK;
		break;
	case UART_OFFSET_DIVS:
		uartReg.divs =
			((uartReg.divs & (~mask)) | (data32 & mask)) & UART_DIVS_MASK;
		break;
	default:
		break;
	}

}

void
uart_write_8(unsigned int address, unsigned char data)
{
	unsigned int addr = address & 0x000000FC;

	unsigned int data32;
	unsigned int mask;

	switch (address & 3) {
	case 0:
		data32 = (unsigned int) data;
		mask = 0x000000FF;
		break;
	case 1:
		data32 = ((unsigned int) data) << 8;
		mask = 0x0000FF00;
		break;
	case 2:
		data32 = ((unsigned int) data) << 16;
		mask = 0x00FF0000;
		break;
	case 3:
		data32 = ((unsigned int) data) << 24;
		mask = 0xFF000000;
		break;
	}

	switch (addr) {
	case UART_OFFSET_DATA:
		uartReg.data =
			((uartReg.data & (~mask)) | (data32 & mask)) & UART_DATA_MASK;
		break;
	case UART_OFFSET_IER:
		uartReg.ier =
			((uartReg.ier & (~mask)) | (data32 & mask)) & UART_IER_MASK;
		break;
	case UART_OFFSET_IIR:
		/* read only. */

		break;
	case UART_OFFSET_LCR:
		uartReg.lcr =
			((uartReg.lcr & (~mask)) | (data32 & mask)) & UART_LCR_MASK;
		break;
	case UART_OFFSET_LSR:
		/* read only. */

		break;
	case UART_OFFSET_DIVR:
		uartReg.divr =
			((uartReg.divr & (~mask)) | (data32 & mask)) & UART_DIVR_MASK;
		break;
	case UART_OFFSET_DIVS:
		uartReg.divs =
			((uartReg.divs & (~mask)) | (data32 & mask)) & UART_DIVS_MASK;
		break;
	default:
		break;
	}

}

/************************************/

void
show_uart_regs(void)
{
	printf("--UART---------------------------------\n");
	printf("$DATA[%08x]", uartReg.data);
	printf("$IER[%08x]", uartReg.ier);
	printf("$IIR[%08x]", uartReg.iir);
	printf("$LCR[%08x]", uartReg.lcr);
	printf("$LSR[%08x]", uartReg.lsr);
	printf("$DIVR[%08x]", uartReg.divr);
	printf("$DIVS[%08x]\n", uartReg.divs);
}
