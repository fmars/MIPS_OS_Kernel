#include<stdio.h>
#include"timer.h"
#include"icu.h"

static struct RTC timerReg[3] = { {0} };

static unsigned int time_ns = 0;
static void timer_ctrl_written(int tunnel);
static void timer_count(int tunnel);

/*******/

void
timer_reset(void)
{
	timerReg[0].ctrl = 0;
	timerReg[1].ctrl = 0;
	timerReg[2].ctrl = 0;
}

void
timer_read_32(unsigned int address, unsigned int *data)
{
	unsigned int addr = address & 0x000000FC;
	unsigned int data32;

	switch (addr) {
	case TIMER_OFFSET_0_CTRL:
		data32 = timerReg[0].ctrl;
		break;
	case TIMER_OFFSET_0_PRESET:
		data32 = timerReg[0].preset;
		break;
	case TIMER_OFFSET_0_COUNT:
		data32 = timerReg[0].count;
		break;
	case TIMER_OFFSET_1_CTRL:
		data32 = timerReg[1].ctrl;
		break;
	case TIMER_OFFSET_1_PRESET:
		data32 = timerReg[1].preset;
		break;
	case TIMER_OFFSET_1_COUNT:
		data32 = timerReg[1].count;
		break;
	case TIMER_OFFSET_2_CTRL:
		data32 = timerReg[2].ctrl;
		break;
	case TIMER_OFFSET_2_PRESET:
		data32 = timerReg[2].preset;
		break;
	case TIMER_OFFSET_2_COUNT:
		data32 = timerReg[2].count;
		break;
	default:
		data32 = 0;
		break;
	}
	*data = data32;
}

void
timer_read_16(unsigned int address, unsigned short *data)
{

	unsigned int addr = address & 0x000000FC;
	unsigned int data32;

	switch (addr) {
	case TIMER_OFFSET_0_CTRL:
		data32 = timerReg[0].ctrl;
		break;
	case TIMER_OFFSET_0_PRESET:
		data32 = timerReg[0].preset;
		break;
	case TIMER_OFFSET_0_COUNT:
		data32 = timerReg[0].count;
		break;
	case TIMER_OFFSET_1_CTRL:
		data32 = timerReg[1].ctrl;
		break;
	case TIMER_OFFSET_1_PRESET:
		data32 = timerReg[1].preset;
		break;
	case TIMER_OFFSET_1_COUNT:
		data32 = timerReg[1].count;
		break;
	case TIMER_OFFSET_2_CTRL:
		data32 = timerReg[2].ctrl;
		break;
	case TIMER_OFFSET_2_PRESET:
		data32 = timerReg[2].preset;
		break;
	case TIMER_OFFSET_2_COUNT:
		data32 = timerReg[2].count;
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
timer_read_8(unsigned int address, unsigned char *data)
{

	unsigned int addr = address & 0x000000FC;
	unsigned int data32;

	switch (addr) {
	case TIMER_OFFSET_0_CTRL:
		data32 = timerReg[0].ctrl;
		break;
	case TIMER_OFFSET_0_PRESET:
		data32 = timerReg[0].preset;
		break;
	case TIMER_OFFSET_0_COUNT:
		data32 = timerReg[0].count;
		break;
	case TIMER_OFFSET_1_CTRL:
		data32 = timerReg[1].ctrl;
		break;
	case TIMER_OFFSET_1_PRESET:
		data32 = timerReg[1].preset;
		break;
	case TIMER_OFFSET_1_COUNT:
		data32 = timerReg[1].count;
		break;
	case TIMER_OFFSET_2_CTRL:
		data32 = timerReg[2].ctrl;
		break;
	case TIMER_OFFSET_2_PRESET:
		data32 = timerReg[2].preset;
		break;
	case TIMER_OFFSET_2_COUNT:
		data32 = timerReg[2].count;
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
timer_write_32(unsigned int address, unsigned int data)
{
	unsigned int addr = address & 0x000000FC;
	unsigned int data32;

	data32 = data;

	switch (addr) {
	case TIMER_OFFSET_0_CTRL:
		timerReg[0].ctrl = data32 & TC_CTRL_MASK;
		timer_ctrl_written(0);
		break;
	case TIMER_OFFSET_0_PRESET:
		timerReg[0].preset = data32;
		break;
	case TIMER_OFFSET_0_COUNT:
		timerReg[0].count = data32;
		break;
	case TIMER_OFFSET_1_CTRL:
		timerReg[1].ctrl = data32 & TC_CTRL_MASK;
		timer_ctrl_written(1);
		break;
	case TIMER_OFFSET_1_PRESET:
		timerReg[1].preset = data32;
		break;
	case TIMER_OFFSET_1_COUNT:
		timerReg[1].count = data32;
		break;
	case TIMER_OFFSET_2_CTRL:
		timerReg[2].ctrl = data32 & TC_CTRL_MASK;
		timer_ctrl_written(2);
		break;
	case TIMER_OFFSET_2_PRESET:
		timerReg[2].preset = data32;
		break;
	case TIMER_OFFSET_2_COUNT:
		timerReg[2].count = data32;
		break;
	default:
		break;
	}

}

void
timer_write_16(unsigned int address, unsigned short data)
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
	case TIMER_OFFSET_0_CTRL:
		timerReg[0].ctrl =
			((timerReg[0].ctrl & (~mask)) | (data32 & mask)) & TC_CTRL_MASK;
		timer_ctrl_written(0);
		break;
	case TIMER_OFFSET_0_PRESET:
		timerReg[0].preset = (timerReg[0].preset & (~mask)) | (data32 & mask);
		break;
	case TIMER_OFFSET_0_COUNT:
		timerReg[0].count = (timerReg[0].count & (~mask)) | (data32 & mask);
		break;
	case TIMER_OFFSET_1_CTRL:
		timerReg[1].ctrl =
			((timerReg[1].ctrl & (~mask)) | (data32 & mask)) & TC_CTRL_MASK;
		timer_ctrl_written(1);
		break;
	case TIMER_OFFSET_1_PRESET:
		timerReg[1].preset = (timerReg[1].preset & (~mask)) | (data32 & mask);
		break;
	case TIMER_OFFSET_1_COUNT:
		timerReg[1].count = (timerReg[1].count & (~mask)) | (data32 & mask);
		break;
	case TIMER_OFFSET_2_CTRL:
		timerReg[2].ctrl =
			((timerReg[2].ctrl & (~mask)) | (data32 & mask)) & TC_CTRL_MASK;
		timer_ctrl_written(2);
		break;
	case TIMER_OFFSET_2_PRESET:
		timerReg[2].preset = (timerReg[2].preset & (~mask)) | (data32 & mask);
		break;
	case TIMER_OFFSET_2_COUNT:
		timerReg[2].count = (timerReg[2].count & (~mask)) | (data32 & mask);
		break;
	default:
		break;
	}

}

void
timer_write_8(unsigned int address, unsigned char data)
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
	default:
		data32 = 0;
		mask = 0x00000000;
		break;
	}

	switch (addr) {
	case TIMER_OFFSET_0_CTRL:
		timerReg[0].ctrl =
			((timerReg[0].ctrl & (~mask)) | (data32 & mask)) & TC_CTRL_MASK;
		timer_ctrl_written(0);
		break;
	case TIMER_OFFSET_0_PRESET:
		timerReg[0].preset = (timerReg[0].preset & (~mask)) | (data32 & mask);
		break;
	case TIMER_OFFSET_0_COUNT:
		timerReg[0].count = (timerReg[0].count & (~mask)) | (data32 & mask);
		break;
	case TIMER_OFFSET_1_CTRL:
		timerReg[1].ctrl =
			((timerReg[1].ctrl & (~mask)) | (data32 & mask)) & TC_CTRL_MASK;
		timer_ctrl_written(1);
		break;
	case TIMER_OFFSET_1_PRESET:
		timerReg[1].preset = (timerReg[1].preset & (~mask)) | (data32 & mask);
		break;
	case TIMER_OFFSET_1_COUNT:
		timerReg[1].count = (timerReg[1].count & (~mask)) | (data32 & mask);
		break;
	case TIMER_OFFSET_2_CTRL:
		timerReg[2].ctrl =
			((timerReg[2].ctrl & (~mask)) | (data32 & mask)) & TC_CTRL_MASK;
		timer_ctrl_written(2);
		break;
	case TIMER_OFFSET_2_PRESET:
		timerReg[2].preset = (timerReg[2].preset & (~mask)) | (data32 & mask);
		break;
	case TIMER_OFFSET_2_COUNT:
		timerReg[2].count = (timerReg[2].count & (~mask)) | (data32 & mask);
		break;
	default:
		break;
	}

}

void
timer_time_pass(unsigned int ns)
{
	/* 1s == 1,000ms ==1,000,000 us == 1,000,000,000 ns */
	time_ns += ns;
	//printf("time : %dns\n",time_ns);
	if (time_ns > TC_TIME_TICK) {
		time_ns -= TC_TIME_TICK;
		// only use tunnel 1
		timer_count(0);
		timer_count(1);
		timer_count(2);
	}
}

static void
timer_ctrl_written(int tunnel)
{
	if (timerReg[tunnel].ctrl & TC_CTRL_ENABLE_MASK)
		timerReg[tunnel].count = timerReg[tunnel].preset;
}

/* int: l-->h */
/* send INT signal in post edge. */
static void
timer_count(int tunnel)
{
	if ((timerReg[tunnel].ctrl & TC_CTRL_ENABLE_MASK) == 0)
		return;

	switch ((timerReg[tunnel].ctrl) & TC_CTRL_MODE_MASK) {
	case 0:
		/*
		   if (timerReg[tunnel].preset == < 1)
		   break;
		 */
		/* sub-count and stop. */
		if (timerReg[tunnel].count) {
			timerReg[tunnel].out = 0;
			(timerReg[tunnel].count)--;
		}
		if (timerReg[tunnel].count == 0 && timerReg[tunnel].out) {
			timerReg[tunnel].out = 1;
			signal_interrupt(7 - tunnel);
			timerReg[tunnel].ctrl &= 0xFFFFFFFE;
		}
		break;
	case 2:
		/*
		   if (timerReg[tunnel].preset == < 1)
		   break;
		 */
		/* rotate sub-count and reset. */

		(timerReg[tunnel].count)--;
		if (timerReg[tunnel].count == 0) {
			timerReg[tunnel].out = 0;
			timerReg[tunnel].count = timerReg[tunnel].preset;
		} else {
			if (timerReg[tunnel].out == 0) {
				timerReg[tunnel].out = 1;
				signal_interrupt(7 - tunnel);
			}
		}
		break;
	case 4:
		/*
		   if (timerReg[tunnel].preset == < 3)
		   break;
		 */
		/* if .count is odd. */
		if (timerReg[tunnel].count & 1) {
			if (timerReg[tunnel].out) {
				/* if out is H. */
				timerReg[tunnel].count -= 1;
			} else {
				/* if out is L. */
				timerReg[tunnel].count -= 3;
			}
		} else {
			timerReg[tunnel].count -= 2;
		}

		if (timerReg[tunnel].count == 0) {
			timerReg[tunnel].count = timerReg[tunnel].preset;
			timerReg[tunnel].out = 1 - timerReg[tunnel].out;
			if (timerReg[tunnel].out) {
				signal_interrupt(7 - tunnel);
			}
		}
		break;
	case 6:
		/* undefined mode */
		break;
	}
}

void
show_timer_regs()
{
	int i;
	printf("--TIMER--------------------------------\n");
	for (i = 0; i < 3; i++) {
		printf("[%d]$CTRL[%08x]", i, timerReg[i].ctrl);
		printf("$PRESET[%08x]", timerReg[i].preset);
		printf("$COUNT[%08x]", timerReg[i].count);
		printf("$OUT[%01x]\n", timerReg[i].out);
	}
}
