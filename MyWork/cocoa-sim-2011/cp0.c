/*
 * cp0.c
 * by wuxb
 */

#include <stdio.h>
#include <string.h>

#include "cp0.h"
#include "cpu.h"
#include "icu.h"

/* cpu_pc in 'cpu.c' */
extern unsigned int cpu_pc;

/* cpu_pc_inst in 'cpu.c' */
extern unsigned int cpu_pc_inst;

static struct RCP0 cp0Reg;

/* flag of exceptions. */
static unsigned int exc_flag = 0;

/* 6 line of hard interrupt interface. */
static unsigned int (*int_checker[6]) (void) = {
NULL};

/* exccode enable check. have address exceptions. */
/*static const unsigned char exccode_accept[32] = {*/
/*	1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0,*/
/*	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0*/
/*};*/
static const unsigned int exccode_accept_way = 0x00001731u;

/* For real hardware, no address exceptions. */
/*
static const unsigned char exccode_accept[32] = {
	1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
*/

void
cp0_reset()
{
	memset(&cp0Reg, 0, sizeof (struct RCP0));
	cp0Reg.sr = CP0_SR_BEV_MASK;
	cp0Reg.prid = CP0_PRID;

	// link icu IRQ4.
	int_checker[2] = check_icu_interrupt;
}

void
read_cp0_reg(unsigned int id, unsigned int *value)
{
	switch (id) {
	case CP0_REGID_SR:
		*value = cp0Reg.sr;
		break;
	case CP0_REGID_CAUSE:
		*value = cp0Reg.cause;
		break;
	case CP0_REGID_EPC:
		*value = cp0Reg.epc;
		break;
	case CP0_REGID_PRID:
		*value = cp0Reg.prid;
		break;
	case CP0_REGID_BADVADDR:
		*value = cp0Reg.badvaddr;
		break;
	default:
		*value = 0;
		break;
	}
}

void
write_cp0_reg(unsigned int id, unsigned int value)
{
	switch (id) {
	case CP0_REGID_SR:
		cp0Reg.sr = value & CP0_SR_MASK;
		break;
	case CP0_REGID_CAUSE:
		/* write cause:[9..8] to cause a software interrupt. */
		cp0Reg.cause = (cp0Reg.cause & (~CP0_CAUSE_MASK_W)) |
			(value & CP0_CAUSE_MASK_W);
		break;
	case CP0_REGID_EPC:
		cp0Reg.epc = value;
		break;
	case CP0_REGID_PRID:
		break;
	case CP0_REGID_BADVADDR:
		break;
	default:
		/* no these reg. */
		break;
	}
	return;
}

/* exception caused by CPU (instructions). */
void
signal_exception(unsigned int exccode, unsigned int epc, unsigned int badvaddr)
{
	//printf("Exception on : %x\n", epc);
/*	if (exccode_accept[exccode & 0x1f] == 0)*/
/*		return;*/
	if ((exccode_accept_way & (1u << (exccode ? (exccode - 1) : 0))) == 0)
		return;

	/* test sr[IE] to ignore exceptions. */
	if ((cp0Reg.sr & CP0_SR_IE_MASK) == 0) {
		//printf("cannot accept this exception.\n");
		return;
	}

	/* set exception flag. */
	exc_flag = 1;

	/* disable anymore exceptions. */
	cp0Reg.sr |= CP0_SR_IE_MASK;
	cp0Reg.sr ^= CP0_SR_IE_MASK;
	/* set address exception's badvaddr.(no mmu, should call badaddr.) */
	if (exccode == CP0_CAUSE_EC_ADEL || exccode == CP0_CAUSE_EC_ADES) {
		cp0Reg.badvaddr = badvaddr;
	}
	/* if we have pipelines, epc depends on slots... */

	cp0Reg.epc = epc;

	cp0Reg.cause = (cp0Reg.cause & (~CP0_CAUSE_EC_MASK)) |
		((exccode << CP0_CAUSE_EC_SHIFT) & CP0_CAUSE_EC_MASK);
	return;
}

/* called by cpu. do anything here. */
void
check_cp0_exception()
{
/* only do one time in one cycle. */
	unsigned int exccode;
	unsigned int bev;
	unsigned int entry;
	unsigned int ircode;

	int i;

	/* no exception, check interrupt. priority:[7..0] */
	if (!exc_flag) {

		/* ~IE dissable all interrupt. */
		if ((cp0Reg.sr & CP0_SR_IE_MASK) == 0)
			return;
		/* check hardware interrupt. [5..0]. */
		for (i = 5; i >= 0; i--) {
			if (int_checker[i] == NULL)
				continue;

			//printf("fmars have int cheker = %d\n",i);
			ircode = (int_checker[i]) ();
			if (ircode == 0)
				continue;
			//printf("fmars check ircode ok = %d\n",ircode);

			exc_flag = i + 10;
			break;
		}
		//fmars
		//if (exc_flag)
		//	printf("fmars check cp0 ok = %d\n", exc_flag);

		/* check software interrupt 1. */
		if ((!exc_flag) && (cp0Reg.cause & 0x00000200u))
			exc_flag = 9;

		/* check software interrupt 0. */
		if ((!exc_flag) && (cp0Reg.cause & 0x00000100u))
			exc_flag = 8;

		/* no INT, check finished. */
		if (!exc_flag)
			return;

		/* interrupt.
		 * exccode = 0;
		 * cause(IP[7..2]) = ircode.
		 * epc = pc_inst.
		 */

		/* cause[exccode] = CP0_CAUSE_EC_INT (0) */
		cp0Reg.cause &= (~CP0_CAUSE_EC_MASK);

		/* set cause[ip]. */

		cp0Reg.cause |= (1 << exc_flag);
		cp0Reg.epc = cpu_pc_inst;
	}
	//fmars
	//printf("fmars check cp0 ok\n");

	/* mask exception. */
	cp0Reg.sr |= CP0_SR_IE_MASK;
	cp0Reg.sr ^= CP0_SR_IE_MASK;
	/* Get BEV */
	bev = cp0Reg.sr & CP0_SR_BEV_MASK;

	/* Get exccode */
	exccode = (cp0Reg.cause & CP0_CAUSE_EC_MASK) << CP0_CAUSE_EC_SHIFT;

	/* decide the entry. */
	if (bev) {
		if (exccode == CP0_CAUSE_EC_INT) {
			/* bev == 1, interrupt. */
			entry = ENTRY_INT_BEV_1;
		} else {
			/* bev == 1, exception. */
			entry = ENTRY_EX_BEV_1;
		}
	} else {
		if (exccode == CP0_CAUSE_EC_INT) {
			/* bev == 0, interrupt */
			entry = ENTRY_INT_BEV_0;
		} else {
			/* bev == 0, exception. */
			entry = ENTRY_EX_BEV_0;
		}
	}

	cpu_pc = entry;
	exc_flag = 0;
	return;
}

/* called by instruction "eretx". set IE and return pc. */
unsigned int
exception_return()
{

	cp0Reg.sr |= CP0_SR_IE_MASK;
	return cp0Reg.epc;
}

void
show_cp0_regs()
{
	printf("--CP0----------------------------------\n");
	printf("$SR[%08x]: ", cp0Reg.sr);
	printf("BEV[%x], ", (cp0Reg.sr & CP0_SR_BEV_MASK) >> CP0_SR_BEV_SHIFT);
	printf("IM[%02x], ", (cp0Reg.sr & CP0_SR_IM_MASK) >> CP0_SR_IM_SHIFT);
	printf("IE[%x]\n", (cp0Reg.sr & CP0_SR_IE_MASK));

	printf("$CAUSE[%08x]: ", cp0Reg.cause);
	printf("IP[%02x], ",
		   (cp0Reg.cause & CP0_CAUSE_IP_MASK) >> CP0_CAUSE_IP_SHIFT);
	printf("EC[%02x]\n",
		   (cp0Reg.cause & CP0_CAUSE_EC_MASK) >> CP0_CAUSE_EC_SHIFT);

	printf("$EPC[%08x], ", cp0Reg.epc);
	printf("$PRID[%08x], ", cp0Reg.prid);
	printf("$BADVADDR[%08x]\n", cp0Reg.badvaddr);

}
