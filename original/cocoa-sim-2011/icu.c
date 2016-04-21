/*
 * icu.c
 * by wuxb
 */
#include <stdio.h>
#include <stdlib.h>
#include "icu.h"
#include "cp0.h"
#include "cpu.h"

static struct RICU icuReg = { 0 };
static unsigned int prepare_ircode;
static void update_interrupt(void);
static void iso_written(void);
/*static void update_interrupt_0(void);*/

void
icu_read_32(unsigned int address, unsigned int *data)
{

	unsigned int addr = address & 0x000000FC;

/*	update_interrupt();*/
	switch (addr) {
	case ICU_OFFSET_IRR:
		*data = icuReg.irr;
		break;
	case ICU_OFFSET_IMR:
		*data = icuReg.imr;
		break;
	case ICU_OFFSET_ISR:
		*data = icuReg.isr;
		break;
	default:
		*data = 0;
		break;
	}
}

void
icu_read_16(unsigned int address, unsigned short *data)
{

	unsigned int addr = address & 0x000000FC;
	unsigned int data32;

/*	update_interrupt();*/
	switch (addr) {
	case ICU_OFFSET_IRR:
		data32 = icuReg.irr;
		break;
	case ICU_OFFSET_IMR:
		data32 = icuReg.imr;
		break;
	case ICU_OFFSET_ISR:
		data32 = icuReg.isr;
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
icu_read_8(unsigned int address, unsigned char *data)
{

	unsigned int addr = address & 0x000000FC;
	unsigned int data32;

/*	update_interrupt();*/
	switch (addr) {
	case ICU_OFFSET_IRR:
		data32 = icuReg.irr;
		break;
	case ICU_OFFSET_IMR:
		data32 = icuReg.imr;
		break;
	case ICU_OFFSET_ISR:
		data32 = icuReg.isr;
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
icu_write_32(unsigned int address, unsigned int data)
{

	unsigned int addr = address & 0x000000FC;

	switch (addr) {
	case ICU_OFFSET_IRR:
		/* read-only. */
		break;
	case ICU_OFFSET_IMR:
		icuReg.imr = data;
		break;
	case ICU_OFFSET_ISR:
		icuReg.isr = (icuReg.isr & (~ICU_ISR_MASK_WRITE)) |
			(data & ICU_ISR_MASK_WRITE);
		iso_written();
		break;
	default:
		/* not defined, keep quiet. */
		return;
		break;
	}
	update_interrupt();

}

void
icu_write_16(unsigned int address, unsigned short data)
{

	unsigned int addr = address & 0x000000FC;
	unsigned int temp;
	unsigned int mask;

	if (address & 2) {
		temp = ((unsigned int) data) << 16;
		mask = 0xFFFF0000;
	} else {
		temp = (unsigned int) data;
		mask = 0x0000FFFF;
	}

	switch (addr) {
	case ICU_OFFSET_IRR:
		/* read-only. */
		break;
	case ICU_OFFSET_IMR:
		icuReg.imr = (icuReg.imr & (~mask)) | (temp & mask);
		break;
	case ICU_OFFSET_ISR:
		icuReg.isr = (icuReg.isr & (~(ICU_ISR_MASK_WRITE & mask))) |
			(data & ICU_ISR_MASK_WRITE & mask);
		iso_written();
		break;
	default:
		/* not defined, keep quiet. */
		return;
		break;
	}
	update_interrupt();

}

void
icu_write_8(unsigned int address, unsigned char data)
{

	unsigned int addr = address & 0x000000FC;
	unsigned int temp;
	unsigned int mask;

	switch (address & 3) {
	case 0:
		temp = (unsigned int) data;
		mask = 0x000000FF;
		break;
	case 1:
		temp = ((unsigned int) data) << 8;
		mask = 0x0000FF00;
		break;
	case 2:
		temp = ((unsigned int) data) << 16;
		mask = 0x00FF0000;
		break;
	case 3:
		temp = ((unsigned int) data) << 24;
		mask = 0xFF000000;
		break;
	default:
		temp = 0;
		mask = 0x00000000;
	}

	switch (addr) {
	case ICU_OFFSET_IRR:
		/* read-only. */
		break;
	case ICU_OFFSET_IMR:
		icuReg.imr = (icuReg.imr & (~mask)) | (temp & mask);
		break;
	case ICU_OFFSET_ISR:
		icuReg.isr = (icuReg.isr & (~(ICU_ISR_MASK_WRITE & mask))) |
			(data & ICU_ISR_MASK_WRITE & mask);
		iso_written();
		break;
	default:
		/* not defined, keep quiet. */
		return;
		break;
	}
	update_interrupt();
}

//////////////////////
void
icu_reset(void)
{
	icuReg.irr = 0x00000000;
	icuReg.imr = 0x000000FF;
	icuReg.isr = 0x00000000;
	prepare_ircode = 0;
}

/* for device call. */
void
signal_interrupt(unsigned int intcode)
{
	unsigned int irmap;

	if (intcode > 7)
		return;

	irmap = 1 << intcode;
	icuReg.irr |= irmap;
	update_interrupt();
}

/*unsigned int
check_icu_interrupt_0(void)
{
	unsigned int ircode;
	unsigned int irr_clean_mask = 0;

	// update enable status.
	update_interrupt();
	ircode = (icuReg.irr & ICU_IRR_IRCODE_MASK) >> ICU_IRR_IRCODE_SHIFT;

	/ * 0 < ircode < 8 * /
	/ * TODO : clean irmap. ?? * /
	if (ircode) {
		/ * here accept the interrupt. * /
		//printf("INT! ircode : %d\n",ircode);
		irr_clean_mask = 1 << (ircode - 1);
		icuReg.irr &= (~irr_clean_mask);
		icuReg.isr |= irr_clean_mask;
	}
	return ircode;
}*/

/* when irmap or imr is changed, update registers. */
/*static void
update_interrupt_0(void)
{
	unsigned int irmap;
	unsigned int irmmap;
	unsigned int ircode;
	unsigned int temp;

	// im == 0, enabled.
	irmmap = (icuReg.irr & ICU_IRR_IRMAP_MASK & (~icuReg.imr));

	ircode = 0;
	temp = irmmap;
	while (temp) {
		ircode++;
		temp >>= 1;
	}
	irmap = icuReg.irr & ICU_IRR_IRMAP_MASK;

	icuReg.irr = ((ircode << ICU_IRR_IRCODE_SHIFT) & ICU_IRR_IRCODE_MASK) |
		((irmmap << ICU_IRR_IRMMAP_SHIFT) & ICU_IRR_IRMMAP_MASK) |
		((irmap << ICU_IRR_IRMAP_SHIFT) & ICU_IRR_IRMAP_MASK);
}*/

unsigned int
check_icu_interrupt(void)
{
	unsigned int ircode;
	unsigned int irr_clean_mask;

	// update enable status.

	/* 0 < ircode < 8 */
	/* TODO : clean irmap. ?? */
	if (prepare_ircode == 0) {
		return 0;
	} else {
		ircode = prepare_ircode;
		/* here accept the interrupt. */
		//printf("INT! ircode : %d\n",ircode);
		irr_clean_mask = 1 << (ircode - 1);
		icuReg.irr &= (~irr_clean_mask);
		icuReg.isr |= irr_clean_mask;
		update_interrupt();
		return ircode;
	}
}

/* when irmap or imr is changed, update registers. */
static void
update_interrupt(void)
{
	unsigned int irmap;
	unsigned int irmmap;
	unsigned int ircode;
	unsigned int temp;

	// im == 0, enabled.
	irmmap = (icuReg.irr & ICU_IRR_IRMAP_MASK & (~icuReg.imr));

	ircode = 0;
	temp = irmmap;
	while (temp) {
		ircode++;
		temp >>= 1;
	}
	irmap = icuReg.irr & ICU_IRR_IRMAP_MASK;

	icuReg.irr = ((ircode << ICU_IRR_IRCODE_SHIFT) & ICU_IRR_IRCODE_MASK) |
		((irmmap << ICU_IRR_IRMMAP_SHIFT) & ICU_IRR_IRMMAP_MASK) |
		((irmap << ICU_IRR_IRMAP_SHIFT) & ICU_IRR_IRMAP_MASK);
	prepare_ircode = ircode;
}

static void
iso_written(void)
{
	unsigned int iso;
	unsigned int isr = 1;

	iso = (icuReg.isr & ICU_ISR_ISO_MASK) >> ICU_ISR_ISO_SHIFT;

	while (iso) {
		isr <<= 1;
		iso--;
	}

	icuReg.isr &= (~isr);
}

void
show_icu_regs(void)
{
	printf("--ICU----------------------------------\n");

	printf("$IRR[%08x] :", icuReg.irr);
	printf("IRCODE[%01x], ",
		   (icuReg.irr & ICU_IRR_IRCODE_MASK) >> ICU_IRR_IRCODE_SHIFT);
	printf("IRMMAP[%02x], ",
		   (icuReg.irr & ICU_IRR_IRMMAP_MASK) >> ICU_IRR_IRMMAP_SHIFT);
	printf("IRMAP[%02x]\n",
		   (icuReg.irr & ICU_IRR_IRMAP_MASK) >> ICU_IRR_IRMAP_SHIFT);

	printf("$IMR[%08x] :IMR[%02x]\n", icuReg.imr, icuReg.imr);

	printf("$ISR[%08x] :", icuReg.isr);
	printf("ISO[%01x], ", (icuReg.isr & ICU_ISR_ISO_MASK) >> ICU_ISR_ISO_SHIFT);
	printf("ISR[%02x]\n", (icuReg.isr & ICU_ISR_ISR_MASK) >> ICU_ISR_ISR_SHIFT);
}
