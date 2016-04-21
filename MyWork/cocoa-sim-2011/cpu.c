/*
 * cpu.c
 * by wuxb
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cpu.h"
#include "cp0.h"
#include "address.h"
#include "timer.h"
#include "flash.h"
#include "disass.h"

/*#define DECODING_MODE_1*/

unsigned int cpu_pc;
unsigned int cpu_pc_inst = 0;
int cpu_big_endian = 0;

static struct RCPU cpuReg;

/* written cpu reg */
static unsigned int wreg = 0;

/* written cp0 reg */
static unsigned int wcp0 = 0;

/* current fetched instruction. */
static unsigned int inst = 0;

/* splited inst ways. */
static unsigned int i_rs = 0;	// also 'base'
static unsigned int i_rt = 0;
static unsigned int i_rd = 0;
static unsigned int i_imm = 0;	// also 'offset'
static unsigned int i_jaddr = 0;
static unsigned int i_sa = 0;

static void execute_inst_3(void);
static void read_cpu_gpr(unsigned int id, unsigned int *data);
static void write_cpu_gpr(unsigned int id, unsigned int data);

static void execute_add(void);
static void execute_addi(void);
static void execute_addu(void);
static void execute_addiu(void);

static void execute_and(void);
static void execute_andi(void);

static void execute_div(void);
static void execute_divu(void);

static void execute_mult(void);
static void execute_multu(void);

static void execute_nor(void);

static void execute_or(void);
static void execute_ori(void);

static void execute_sll(void);
static void execute_sllv(void);
static void execute_sra(void);
static void execute_srav(void);
static void execute_srl(void);
static void execute_srlv(void);

static void execute_sub(void);
static void execute_subu(void);

static void execute_xor(void);
static void execute_xori(void);

static void execute_lui(void);

static void execute_slt(void);
static void execute_slti(void);
static void execute_sltu(void);
static void execute_sltiu(void);

static void execute_beq(void);
static void execute_bgez(void);
static void execute_bgezal(void);
static void execute_bgtz(void);
static void execute_blez(void);
static void execute_bltz(void);
static void execute_bltzal(void);
static void execute_bne(void);

static void execute_j(void);
static void execute_jal(void);
static void execute_jalr(void);
static void execute_jr(void);

static void execute_lb(void);
static void execute_lbu(void);
static void execute_lh(void);
static void execute_lhu(void);
static void execute_lw(void);

static void execute_sb(void);
static void execute_sh(void);
static void execute_sw(void);

static void execute_break(void);
static void execute_syscall(void);

static void execute_mfhi(void);
static void execute_mflo(void);
static void execute_mthi(void);
static void execute_mtlo(void);

static void execute_mfc0(void);
static void execute_mtc0(void);

static void execute_eretx(void);

static void execute_null(void);

/* decode */

static void (*exec_root[64]) (void) = {
		execute_null, execute_null, execute_j, execute_jal,
		execute_beq, execute_bne, execute_blez, execute_bgtz,
		execute_addi, execute_addiu, execute_slti, execute_sltiu,
		execute_andi, execute_ori, execute_xori, execute_lui,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_lb, execute_lh, execute_null, execute_lw,
		execute_lbu, execute_lhu, execute_null, execute_null,
		execute_sb, execute_sh, execute_null, execute_sw,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null};

static void (*exec_t00[64]) (void) = {
		execute_sll, execute_null, execute_srl, execute_sra,
		execute_sllv, execute_null, execute_srlv, execute_srav,
		execute_jr, execute_jalr, execute_null, execute_null,
		execute_syscall, execute_break, execute_null, execute_null,
		execute_mfhi, execute_mthi, execute_mflo, execute_mtlo,
		execute_null, execute_null, execute_null, execute_null,
		execute_mult, execute_multu, execute_div, execute_divu,
		execute_null, execute_null, execute_null, execute_null,
		execute_add, execute_addu, execute_sub, execute_subu,
		execute_and, execute_or, execute_xor, execute_nor,
		execute_null, execute_null, execute_slt, execute_sltu,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null};

static void (*exec_t01[32]) (void) = {
		execute_bltz, execute_bgez, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_bltzal, execute_bgezal, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null};

static void (*exec_t10[32]) (void) = {
		execute_mfc0, execute_null, execute_null, execute_null,
		execute_mtc0, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_eretx, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null,
		execute_null, execute_null, execute_null, execute_null};

/* */
static unsigned int fetch_inst(void);

/*#ifdef DECODING_MODE_1*/
static void execute_inst_1(void);
/*#else*/
/*static void execute_inst_2(void);*/
/*static void init_decode_table_2(void);*/
/*static struct DECODE *decode = NULL;*/
/*static unsigned int init_decode_mask = INST_MASK_31_26;*/
/*static unsigned int init_decode_shift = 26;*/
/*#endif*/

void
cpu_reset(void)
{
	memset((void *) &cpuReg, 0, sizeof (struct RCPU));
	cpu_pc = PC_ADDRESS_RESET;
/*#ifndef DECODING_MODE_1*/
/*	init_decode_table_2();*/
/*#endif*/
	return;
}

void
execute_cycle(void)
{
	unsigned int exccode;
	wreg = 0;
	wcp0 = 0;
	exccode = fetch_inst();
	if (exccode) {
		signal_exception(exccode, cpu_pc_inst, cpu_pc_inst);
	} else {
		//printf("inst @ [%08x] %08X\n", inst, cpu_pc_inst);
/*#ifdef DECODING_MODE_1*/
		execute_inst_1();
/*#else*/
/*		execute_inst_2();*/
/*#endif*/
	}

	// Timer's work.
	timer_time_pass(TC_TIME_INST_COMMON);
	// TODO : UART's work ( not implemented ).
	//  finally, cp0's work, cp0 will check icu to update INT status.
	check_cp0_exception();
}

/*#ifdef DECODING_MODE_1*/
/* check instruction and execute. */
static void
execute_inst_1(void)
{
	int bad_inst = 0;
	switch (inst & INST_MASK_31_26) {
	case INST_MASK_31_26_x00:
		switch (inst & INST_MASK_5_0) {
		case INST_MASK_5_0_ADD:
			if (INST_CHECK(inst, INST_MASK_ADD, INST_CHECK_ADD))
				execute_add();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_ADDU:
			if (INST_CHECK(inst, INST_MASK_ADDU, INST_CHECK_ADDU))
				execute_addu();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_AND:
			if (INST_CHECK(inst, INST_MASK_AND, INST_CHECK_AND))
				execute_and();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_DIV:
			if (INST_CHECK(inst, INST_MASK_DIV, INST_CHECK_DIV))
				execute_div();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_DIVU:
			if (INST_CHECK(inst, INST_MASK_DIVU, INST_CHECK_DIVU))
				execute_divu();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_MULT:
			if (INST_CHECK(inst, INST_MASK_MULT, INST_CHECK_MULT))
				execute_mult();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_MULTU:
			if (INST_CHECK(inst, INST_MASK_MULTU, INST_CHECK_MULTU))
				execute_multu();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_NOR:
			if (INST_CHECK(inst, INST_MASK_NOR, INST_CHECK_NOR))
				execute_nor();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_OR:
			if (INST_CHECK(inst, INST_MASK_OR, INST_CHECK_OR))
				execute_or();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_SLL:
			if (INST_CHECK(inst, INST_MASK_SLL, INST_CHECK_SLL))
				execute_sll();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_SLLV:
			if (INST_CHECK(inst, INST_MASK_SLLV, INST_CHECK_SLLV))
				execute_sllv();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_SRA:
			if (INST_CHECK(inst, INST_MASK_SRA, INST_CHECK_SRA))
				execute_sra();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_SRAV:
			if (INST_CHECK(inst, INST_MASK_SRAV, INST_CHECK_SRAV))
				execute_srav();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_SRL:
			if (INST_CHECK(inst, INST_MASK_SRL, INST_CHECK_SRL))
				execute_srl();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_SRLV:
			if (INST_CHECK(inst, INST_MASK_SRLV, INST_CHECK_SRLV))
				execute_srlv();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_SUB:
			if (INST_CHECK(inst, INST_MASK_SUB, INST_CHECK_SUB))
				execute_sub();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_SUBU:
			if (INST_CHECK(inst, INST_MASK_SUBU, INST_CHECK_SUBU))
				execute_subu();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_XOR:
			if (INST_CHECK(inst, INST_MASK_XOR, INST_CHECK_XOR))
				execute_xor();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_SLT:
			if (INST_CHECK(inst, INST_MASK_SLT, INST_CHECK_SLT))
				execute_slt();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_SLTU:
			if (INST_CHECK(inst, INST_MASK_SLTU, INST_CHECK_SLTU))
				execute_sltu();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		/*case INST_MASK_5_0_JALR:
			if (INST_CHECK(inst, INST_MASK_JALR, INST_CHECK_JALR))
				execute_jalr();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
        */
		case INST_MASK_5_0_JR:
			if (INST_CHECK(inst, INST_MASK_JR, INST_CHECK_JR))
				execute_jr();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_MFHI:
			if (INST_CHECK(inst, INST_MASK_MFHI, INST_CHECK_MFHI))
				execute_mfhi();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_MFLO:
			if (INST_CHECK(inst, INST_MASK_MFLO, INST_CHECK_MFLO))
				execute_mflo();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		/*case INST_MASK_5_0_MTHI:
			if (INST_CHECK(inst, INST_MASK_MTHI, INST_CHECK_MTHI))
				execute_mthi();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_MTLO:
			if (INST_CHECK(inst, INST_MASK_MTLO, INST_CHECK_MTLO))
				execute_mtlo();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
        */
		case INST_MASK_5_0_SYSCALL:
			if (INST_CHECK(inst, INST_MASK_SYSCALL, INST_CHECK_SYSCALL))
				execute_syscall();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_5_0_BREAK:
			if (INST_CHECK(inst, INST_MASK_BREAK, INST_CHECK_BREAK))
				execute_break();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		}
		break;
	case INST_MASK_31_26_x01:
		switch (inst & INST_MASK_20_16) {
		case INST_MASK_20_16_BGEZ:
			if (INST_CHECK(inst, INST_MASK_BGEZ, INST_CHECK_BGEZ))
				execute_bgez();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		/*case INST_MASK_20_16_BGEZAL:
			if (INST_CHECK(inst, INST_MASK_BGEZAL, INST_CHECK_BGEZAL))
				execute_bgezal();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_20_16_BLTZAL:
			if (INST_CHECK(inst, INST_MASK_BLTZAL, INST_CHECK_BLTZAL))
				execute_bltzal();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
        */
		case INST_MASK_20_16_BLTZ:
			if (INST_CHECK(inst, INST_MASK_BLTZ, INST_CHECK_BLTZ))
				execute_bltz();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		}
		break;
	case INST_MASK_31_26_x10:
		switch (inst & INST_MASK_25_21) {
		case INST_MASK_25_21_MFC0:
			if (INST_CHECK(inst, INST_MASK_MFC0, INST_CHECK_MFC0))
				execute_mfc0();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_25_21_MTC0:
			if (INST_CHECK(inst, INST_MASK_MTC0, INST_CHECK_MTC0))
				execute_mtc0();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		case INST_MASK_25_21_ERETX:
			if (INST_CHECK(inst, INST_MASK_ERETX, INST_CHECK_ERETX))
				execute_eretx();
			else
				bad_inst = CP0_CAUSE_EC_RI;
			break;
		}
		break;
	case INST_MASK_31_26_ADDI:
		if (INST_CHECK(inst, INST_MASK_ADDI, INST_CHECK_ADDI))
			execute_addi();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_ADDIU:
		if (INST_CHECK(inst, INST_MASK_ADDIU, INST_CHECK_ADDIU))
			execute_addiu();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_ANDI:
		if (INST_CHECK(inst, INST_MASK_ANDI, INST_CHECK_ANDI))
			execute_andi();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_ORI:
		if (INST_CHECK(inst, INST_MASK_ORI, INST_CHECK_ORI))
			execute_ori();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_XORI:
		if (INST_CHECK(inst, INST_MASK_XORI, INST_CHECK_XORI))
			execute_xori();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_LUI:
		if (INST_CHECK(inst, INST_MASK_LUI, INST_CHECK_LUI))
			execute_lui();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_SLTI:
		if (INST_CHECK(inst, INST_MASK_SLTI, INST_CHECK_SLTI))
			execute_slti();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_SLTIU:
		if (INST_CHECK(inst, INST_MASK_SLTIU, INST_CHECK_SLTIU))
			execute_sltiu();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_BEQ:
		if (INST_CHECK(inst, INST_MASK_BEQ, INST_CHECK_BEQ))
			execute_beq();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_BGTZ:
		if (INST_CHECK(inst, INST_MASK_BGTZ, INST_CHECK_BGTZ))
			execute_bgtz();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_BLEZ:
		if (INST_CHECK(inst, INST_MASK_BLEZ, INST_CHECK_BLEZ))
			execute_blez();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_BNE:
		if (INST_CHECK(inst, INST_MASK_BNE, INST_CHECK_BNE))
			execute_bne();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_J:
		if (INST_CHECK(inst, INST_MASK_J, INST_CHECK_J))
			execute_j();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_JAL:
		if (INST_CHECK(inst, INST_MASK_JAL, INST_CHECK_JAL))
			execute_jal();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
    /*
	case INST_MASK_31_26_LB:
		if (INST_CHECK(inst, INST_MASK_LB, INST_CHECK_LB))
			execute_lb();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
    */
	case INST_MASK_31_26_LBU:
		if (INST_CHECK(inst, INST_MASK_LBU, INST_CHECK_LBU))
			execute_lbu();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
    /*
	case INST_MASK_31_26_LH:
		if (INST_CHECK(inst, INST_MASK_LH, INST_CHECK_LH))
			execute_lh();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
    */
	case INST_MASK_31_26_LHU:
		if (INST_CHECK(inst, INST_MASK_LHU, INST_CHECK_LHU))
			execute_lhu();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_LW:
		if (INST_CHECK(inst, INST_MASK_LW, INST_CHECK_LW))
			execute_lw();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_SB:
		if (INST_CHECK(inst, INST_MASK_SB, INST_CHECK_SB))
			execute_sb();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_SH:
		if (INST_CHECK(inst, INST_MASK_SH, INST_CHECK_SH))
			execute_sh();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	case INST_MASK_31_26_SW:
		if (INST_CHECK(inst, INST_MASK_SW, INST_CHECK_SW))
			execute_sw();
		else
			bad_inst = CP0_CAUSE_EC_RI;
		break;
	default:
		bad_inst = CP0_CAUSE_EC_RI;
		break;
	}
	if (bad_inst)
		signal_exception(bad_inst, cpu_pc_inst, 0);
}

/*#else*/

/*static void*/
/*execute_inst_2()*/
/*{*/
/*	unsigned int decode_mask = init_decode_mask;*/
/*	unsigned int decode_shift = init_decode_shift;*/
/*	unsigned int index;*/
/*	struct DECODE *temp = decode;*/
/*	//printf("I [%08x]\n", inst);*/
/*	for (;;) {*/
/*		index = (inst & decode_mask) >> decode_shift;*/
/*		temp = &temp[index];*/
/*		//printf("R = 0x%x\n", index);*/
/*		switch (temp->type) {*/
/*		case DECODE_TYPE_OP:*/
/*			//printf("Get op %s\n", temp->name);*/
/*			if ((temp->mask & inst) == temp->check) {*/
/*				temp->op();*/
/*			} else {*/
/*				signal_exception(CP0_CAUSE_EC_RI, cpu_pc_inst, 0);*/
/*			}*/
/*			return;*/
/*		case DECODE_TYPE_NEXT:*/
/*			//printf("Next 0x%x\n", temp->mask_next);*/
/*			decode_mask = temp->mask_next;*/
/*			decode_shift = temp->shift_next;*/
/*			temp = temp->next;*/
/*			break;*/
/*		case DECODE_TYPE_NULL:*/
/*			signal_exception(CP0_CAUSE_EC_RI, cpu_pc_inst, 0);*/
/*			return;*/
/*		}*/
/*	}*/
/*}*/

/*static void*/
/*init_decode_table_2()*/
/*{*/
/*	struct DECODE *temp;*/
/*	// clean if reset */
/*	if (decode) {*/
/*		free(decode[0].next);*/
/*		free(decode[0x10].next);*/
/*		free(decode[1].next);*/
/*		free(decode);*/
/*	}*/

/*	decode = malloc(64 * sizeof (struct DECODE));*/
/*	memset(decode, 0, 64 * sizeof (struct DECODE));*/
/*	init_decode_mask = INST_MASK_31_26;*/
/*	init_decode_shift = 26;*/

/*	// 31_26 */
/*	decode[0].type = DECODE_TYPE_NEXT;*/
/*	temp = malloc(64 * sizeof (struct DECODE));*/
/*	memset(temp, 0, 64 * sizeof (struct DECODE));*/
/*	decode[0].next = temp;*/
/*	decode[0].mask_next = INST_MASK_5_0;*/
/*	decode[0].shift_next = 0;*/

/*	//31_26[0] --> 5_0*/
/*	{*/
/*		temp[0x20].type = DECODE_TYPE_OP;*/
/*		temp[0x20].check = INST_CHECK_ADD;*/
/*		temp[0x20].mask = INST_MASK_ADD;*/
/*		temp[0x20].op = execute_add;*/
/*		temp[0x20].name = "add";*/

/*		temp[0x21].type = DECODE_TYPE_OP;*/
/*		temp[0x21].check = INST_CHECK_ADDU;*/
/*		temp[0x21].mask = INST_MASK_ADDU;*/
/*		temp[0x21].op = execute_addu;*/
/*		temp[0x21].name = "addu";*/

/*		temp[0x24].type = DECODE_TYPE_OP;*/
/*		temp[0x24].check = INST_CHECK_AND;*/
/*		temp[0x24].mask = INST_MASK_AND;*/
/*		temp[0x24].op = execute_and;*/
/*		temp[0x24].name = "and";*/

/*		temp[0x1a].type = DECODE_TYPE_OP;*/
/*		temp[0x1a].check = INST_CHECK_DIV;*/
/*		temp[0x1a].mask = INST_MASK_DIV;*/
/*		temp[0x1a].op = execute_div;*/
/*		temp[0x1a].name = "div";*/

/*		temp[0x1b].type = DECODE_TYPE_OP;*/
/*		temp[0x1b].check = INST_CHECK_DIVU;*/
/*		temp[0x1b].mask = INST_MASK_DIVU;*/
/*		temp[0x1b].op = execute_divu;*/
/*		temp[0x1b].name = "divu";*/

/*		temp[0x18].type = DECODE_TYPE_OP;*/
/*		temp[0x18].check = INST_CHECK_MULT;*/
/*		temp[0x18].mask = INST_MASK_MULT;*/
/*		temp[0x18].op = execute_mult;*/
/*		temp[0x18].name = "mult";*/

/*		temp[0x19].type = DECODE_TYPE_OP;*/
/*		temp[0x19].check = INST_CHECK_MULTU;*/
/*		temp[0x19].mask = INST_MASK_MULTU;*/
/*		temp[0x19].op = execute_multu;*/
/*		temp[0x19].name = "multu";*/

/*		temp[0x27].type = DECODE_TYPE_OP;*/
/*		temp[0x27].check = INST_CHECK_NOR;*/
/*		temp[0x27].mask = INST_MASK_NOR;*/
/*		temp[0x27].op = execute_nor;*/
/*		temp[0x27].name = "nor";*/

/*		temp[0x25].type = DECODE_TYPE_OP;*/
/*		temp[0x25].check = INST_CHECK_OR;*/
/*		temp[0x25].mask = INST_MASK_OR;*/
/*		temp[0x25].op = execute_or;*/
/*		temp[0x25].name = "or";*/

/*		temp[0x00].type = DECODE_TYPE_OP;*/
/*		temp[0x00].check = INST_CHECK_SLL;*/
/*		temp[0x00].mask = INST_MASK_SLL;*/
/*		temp[0x00].op = execute_sll;*/
/*		temp[0x00].name = "sll";*/

/*		temp[0x04].type = DECODE_TYPE_OP;*/
/*		temp[0x04].check = INST_CHECK_SLLV;*/
/*		temp[0x04].mask = INST_MASK_SLLV;*/
/*		temp[0x04].op = execute_sllv;*/
/*		temp[0x04].name = "sllv";*/

/*		temp[0x03].type = DECODE_TYPE_OP;*/
/*		temp[0x03].check = INST_CHECK_SRA;*/
/*		temp[0x03].mask = INST_MASK_SRA;*/
/*		temp[0x03].op = execute_sra;*/
/*		temp[0x03].name = "sra";*/

/*		temp[0x07].type = DECODE_TYPE_OP;*/
/*		temp[0x07].check = INST_CHECK_SRAV;*/
/*		temp[0x07].mask = INST_MASK_SRAV;*/
/*		temp[0x07].op = execute_srav;*/
/*		temp[0x07].name = "srav";*/

/*		temp[0x02].type = DECODE_TYPE_OP;*/
/*		temp[0x02].check = INST_CHECK_SRL;*/
/*		temp[0x02].mask = INST_MASK_SRL;*/
/*		temp[0x02].op = execute_srl;*/
/*		temp[0x02].name = "srl";*/

/*		temp[0x06].type = DECODE_TYPE_OP;*/
/*		temp[0x06].check = INST_CHECK_SRLV;*/
/*		temp[0x06].mask = INST_MASK_SRLV;*/
/*		temp[0x06].op = execute_srlv;*/
/*		temp[0x06].name = "srlv";*/

/*		temp[0x22].type = DECODE_TYPE_OP;*/
/*		temp[0x22].check = INST_CHECK_SUB;*/
/*		temp[0x22].mask = INST_MASK_SUB;*/
/*		temp[0x22].op = execute_sub;*/
/*		temp[0x22].name = "sub";*/

/*		temp[0x23].type = DECODE_TYPE_OP;*/
/*		temp[0x23].check = INST_CHECK_SUBU;*/
/*		temp[0x23].mask = INST_MASK_SUBU;*/
/*		temp[0x23].op = execute_subu;*/
/*		temp[0x23].name = "subu";*/

/*		temp[0x26].type = DECODE_TYPE_OP;*/
/*		temp[0x26].check = INST_CHECK_XOR;*/
/*		temp[0x26].mask = INST_MASK_XOR;*/
/*		temp[0x26].op = execute_xor;*/
/*		temp[0x26].name = "xor";*/

/*		temp[0x2a].type = DECODE_TYPE_OP;*/
/*		temp[0x2a].check = INST_CHECK_SLT;*/
/*		temp[0x2a].mask = INST_MASK_SLT;*/
/*		temp[0x2a].op = execute_slt;*/
/*		temp[0x2a].name = "slt";*/

/*		temp[0x2b].type = DECODE_TYPE_OP;*/
/*		temp[0x2b].check = INST_CHECK_SLTU;*/
/*		temp[0x2b].mask = INST_MASK_SLTU;*/
/*		temp[0x2b].op = execute_sltu;*/
/*		temp[0x2b].name = "sltu";*/

/*		temp[0x09].type = DECODE_TYPE_OP;*/
/*		temp[0x09].check = INST_CHECK_JALR;*/
/*		temp[0x09].mask = INST_MASK_JALR;*/
/*		temp[0x09].op = execute_jalr;*/
/*		temp[0x09].name = "jalr";*/

/*		temp[0x08].type = DECODE_TYPE_OP;*/
/*		temp[0x08].check = INST_CHECK_JR;*/
/*		temp[0x08].mask = INST_MASK_JR;*/
/*		temp[0x08].op = execute_jr;*/
/*		temp[0x08].name = "jr";*/

/*		temp[0x10].type = DECODE_TYPE_OP;*/
/*		temp[0x10].check = INST_CHECK_MFHI;*/
/*		temp[0x10].mask = INST_MASK_MFHI;*/
/*		temp[0x10].op = execute_mfhi;*/
/*		temp[0x10].name = "mfhi";*/

/*		temp[0x12].type = DECODE_TYPE_OP;*/
/*		temp[0x12].check = INST_CHECK_MFLO;*/
/*		temp[0x12].mask = INST_MASK_MFLO;*/
/*		temp[0x12].op = execute_mflo;*/
/*		temp[0x12].name = "mflo";*/

/*		temp[0x11].type = DECODE_TYPE_OP;*/
/*		temp[0x11].check = INST_CHECK_MTHI;*/
/*		temp[0x11].mask = INST_MASK_MTHI;*/
/*		temp[0x11].op = execute_mthi;*/
/*		temp[0x11].name = "mthi";*/

/*		temp[0x13].type = DECODE_TYPE_OP;*/
/*		temp[0x13].check = INST_CHECK_MTLO;*/
/*		temp[0x13].mask = INST_MASK_MTLO;*/
/*		temp[0x13].op = execute_mtlo;*/
/*		temp[0x13].name = "mtlo";*/

/*		temp[0x0c].type = DECODE_TYPE_OP;*/
/*		temp[0x0c].check = INST_CHECK_SYSCALL;*/
/*		temp[0x0c].mask = INST_MASK_SYSCALL;*/
/*		temp[0x0c].op = execute_syscall;*/
/*		temp[0x0c].name = "syscall";*/

/*		temp[0x0d].type = DECODE_TYPE_OP;*/
/*		temp[0x0d].check = INST_CHECK_BREAK;*/
/*		temp[0x0d].mask = INST_MASK_BREAK;*/
/*		temp[0x0d].op = execute_break;*/
/*		temp[0x0d].name = "break";*/
/*	}*/

/*	decode[0x01].type = DECODE_TYPE_NEXT;*/
/*	temp = malloc(32 * sizeof (struct DECODE));*/
/*	memset(temp, 0, 32 * sizeof (struct DECODE));*/
/*	decode[0x01].next = temp;*/
/*	decode[0x01].mask_next = INST_MASK_20_16;*/
/*	decode[0x01].shift_next = 16;*/

/*	{*/
/*		temp[0x01].type = DECODE_TYPE_OP;*/
/*		temp[0x01].check = INST_CHECK_BGEZ;*/
/*		temp[0x01].mask = INST_MASK_BGEZ;*/
/*		temp[0x01].op = execute_bgez;*/
/*		temp[0x01].name = "bgez";*/

/*		temp[0x11].type = DECODE_TYPE_OP;*/
/*		temp[0x11].check = INST_CHECK_BGEZAL;*/
/*		temp[0x11].mask = INST_MASK_BGEZAL;*/
/*		temp[0x11].op = execute_bgezal;*/
/*		temp[0x11].name = "bgezal";*/

/*		temp[0x10].type = DECODE_TYPE_OP;*/
/*		temp[0x10].check = INST_CHECK_BLTZAL;*/
/*		temp[0x10].mask = INST_MASK_BLTZAL;*/
/*		temp[0x10].op = execute_bltzal;*/
/*		temp[0x10].name = "bltzal";*/

/*		temp[0x00].type = DECODE_TYPE_OP;*/
/*		temp[0x00].check = INST_CHECK_BLTZ;*/
/*		temp[0x00].mask = INST_MASK_BLTZ;*/
/*		temp[0x00].op = execute_bltz;*/
/*		temp[0x00].name = "bltz";*/
/*	}*/

/*	decode[0x10].type = DECODE_TYPE_NEXT;*/
/*	temp = malloc(32 * sizeof (struct DECODE));*/
/*	memset(temp, 0, 32 * sizeof (struct DECODE));*/
/*	decode[0x10].next = temp;*/
/*	decode[0x10].mask_next = INST_MASK_25_21;*/
/*	decode[0x10].shift_next = 21;*/

/*	{*/
/*		temp[0x00].type = DECODE_TYPE_OP;*/
/*		temp[0x00].check = INST_CHECK_MFC0;*/
/*		temp[0x00].mask = INST_MASK_MFC0;*/
/*		temp[0x00].op = execute_mfc0;*/
/*		temp[0x00].name = "mfc0";*/

/*		temp[0x04].type = DECODE_TYPE_OP;*/
/*		temp[0x04].check = INST_CHECK_MTC0;*/
/*		temp[0x04].mask = INST_MASK_MTC0;*/
/*		temp[0x04].op = execute_mtc0;*/
/*		temp[0x04].name = "mtc0";*/

/*		temp[0x10].type = DECODE_TYPE_OP;*/
/*		temp[0x10].check = INST_CHECK_ERETX;*/
/*		temp[0x10].mask = INST_MASK_ERETX;*/
/*		temp[0x10].op = execute_eretx;*/
/*		temp[0x10].name = "eretx";*/
/*	}*/
/*	temp = decode;*/

/*	temp[0x08].type = DECODE_TYPE_OP;*/
/*	temp[0x08].check = INST_CHECK_ADDI;*/
/*	temp[0x08].mask = INST_MASK_ADDI;*/
/*	temp[0x08].op = execute_addi;*/
/*	temp[0x08].name = "addi";*/

/*	temp[0x09].type = DECODE_TYPE_OP;*/
/*	temp[0x09].check = INST_CHECK_ADDIU;*/
/*	temp[0x09].mask = INST_MASK_ADDIU;*/
/*	temp[0x09].op = execute_addiu;*/
/*	temp[0x09].name = "addiu";*/

/*	temp[0x0c].type = DECODE_TYPE_OP;*/
/*	temp[0x0c].check = INST_CHECK_ANDI;*/
/*	temp[0x0c].mask = INST_MASK_ANDI;*/
/*	temp[0x0c].op = execute_andi;*/
/*	temp[0x0c].name = "andi";*/

/*	temp[0x0d].type = DECODE_TYPE_OP;*/
/*	temp[0x0d].check = INST_CHECK_ORI;*/
/*	temp[0x0d].mask = INST_MASK_ORI;*/
/*	temp[0x0d].op = execute_ori;*/
/*	temp[0x0d].name = "ori";*/

/*	temp[0x0e].type = DECODE_TYPE_OP;*/
/*	temp[0x0e].check = INST_CHECK_XORI;*/
/*	temp[0x0e].mask = INST_MASK_XORI;*/
/*	temp[0x0e].op = execute_xori;*/
/*	temp[0x0e].name = "xori";*/

/*	temp[0x0f].type = DECODE_TYPE_OP;*/
/*	temp[0x0f].check = INST_CHECK_LUI;*/
/*	temp[0x0f].mask = INST_MASK_LUI;*/
/*	temp[0x0f].op = execute_lui;*/
/*	temp[0x0f].name = "lui";*/

/*	temp[0x0a].type = DECODE_TYPE_OP;*/
/*	temp[0x0a].check = INST_CHECK_SLTI;*/
/*	temp[0x0a].mask = INST_MASK_SLTI;*/
/*	temp[0x0a].op = execute_slti;*/
/*	temp[0x0a].name = "slti";*/

/*	temp[0x0b].type = DECODE_TYPE_OP;*/
/*	temp[0x0b].check = INST_CHECK_SLTIU;*/
/*	temp[0x0b].mask = INST_MASK_SLTIU;*/
/*	temp[0x0b].op = execute_sltiu;*/
/*	temp[0x0b].name = "sltiu";*/

/*	temp[0x04].type = DECODE_TYPE_OP;*/
/*	temp[0x04].check = INST_CHECK_BEQ;*/
/*	temp[0x04].mask = INST_MASK_BEQ;*/
/*	temp[0x04].op = execute_beq;*/
/*	temp[0x04].name = "beq";*/

/*	temp[0x07].type = DECODE_TYPE_OP;*/
/*	temp[0x07].check = INST_CHECK_BGTZ;*/
/*	temp[0x07].mask = INST_MASK_BGTZ;*/
/*	temp[0x07].op = execute_bgtz;*/
/*	temp[0x07].name = "bgtz";*/

/*	temp[0x06].type = DECODE_TYPE_OP;*/
/*	temp[0x06].check = INST_CHECK_BLEZ;*/
/*	temp[0x06].mask = INST_MASK_BLEZ;*/
/*	temp[0x06].op = execute_blez;*/
/*	temp[0x06].name = "blez";*/

/*	temp[0x05].type = DECODE_TYPE_OP;*/
/*	temp[0x05].check = INST_CHECK_BNE;*/
/*	temp[0x05].mask = INST_MASK_BNE;*/
/*	temp[0x05].op = execute_bne;*/
/*	temp[0x05].name = "bne";*/

/*	temp[0x02].type = DECODE_TYPE_OP;*/
/*	temp[0x02].check = INST_CHECK_J;*/
/*	temp[0x02].mask = INST_MASK_J;*/
/*	temp[0x02].op = execute_j;*/
/*	temp[0x02].name = "j";*/

/*	temp[0x03].type = DECODE_TYPE_OP;*/
/*	temp[0x03].check = INST_CHECK_JAL;*/
/*	temp[0x03].mask = INST_MASK_JAL;*/
/*	temp[0x03].op = execute_jal;*/
/*	temp[0x03].name = "jal";*/

/*	temp[0x20].type = DECODE_TYPE_OP;*/
/*	temp[0x20].check = INST_CHECK_LB;*/
/*	temp[0x20].mask = INST_MASK_LB;*/
/*	temp[0x20].op = execute_lb;*/
/*	temp[0x20].name = "lb";*/

/*	temp[0x24].type = DECODE_TYPE_OP;*/
/*	temp[0x24].check = INST_CHECK_LBU;*/
/*	temp[0x24].mask = INST_MASK_LBU;*/
/*	temp[0x24].op = execute_lbu;*/
/*	temp[0x24].name = "lbu";*/

/*	temp[0x21].type = DECODE_TYPE_OP;*/
/*	temp[0x21].check = INST_CHECK_LH;*/
/*	temp[0x21].mask = INST_MASK_LH;*/
/*	temp[0x21].op = execute_lh;*/
/*	temp[0x21].name = "lh";*/

/*	temp[0x25].type = DECODE_TYPE_OP;*/
/*	temp[0x25].check = INST_CHECK_LHU;*/
/*	temp[0x25].mask = INST_MASK_LHU;*/
/*	temp[0x25].op = execute_lhu;*/
/*	temp[0x25].name = "lhu";*/

/*	temp[0x23].type = DECODE_TYPE_OP;*/
/*	temp[0x23].check = INST_CHECK_LW;*/
/*	temp[0x23].mask = INST_MASK_LW;*/
/*	temp[0x23].op = execute_lw;*/
/*	temp[0x23].name = "lw";*/

/*	temp[0x28].type = DECODE_TYPE_OP;*/
/*	temp[0x28].check = INST_CHECK_SB;*/
/*	temp[0x28].mask = INST_MASK_SB;*/
/*	temp[0x28].op = execute_sb;*/
/*	temp[0x28].name = "sb";*/

/*	temp[0x29].type = DECODE_TYPE_OP;*/
/*	temp[0x29].check = INST_CHECK_SH;*/
/*	temp[0x29].mask = INST_MASK_SH;*/
/*	temp[0x29].op = execute_sh;*/
/*	temp[0x29].name = "sh";*/

/*	temp[0x2b].type = DECODE_TYPE_OP;*/
/*	temp[0x2b].check = INST_CHECK_SW;*/
/*	temp[0x2b].mask = INST_MASK_SW;*/
/*	temp[0x2b].op = execute_sw;*/
/*	temp[0x2b].name = "sw";*/
/*}*/

/*#endif*/

static void
execute_inst_3(void)
{
	void (*exec) (void);
	unsigned int temp;
	temp = inst & INST_MASK_31_26;
	switch (temp) {
	case INST_MASK_31_26_x00:
		exec = (exec_t00[INST_SPLIT_5_0(inst)]);
		break;
	case INST_MASK_31_26_x01:
		exec = (exec_t01[INST_SPLIT_20_16(inst)]);
		break;
	case INST_MASK_31_26_x10:
		exec = (exec_t10[INST_SPLIT_25_21(inst)]);
		break;
	default:
		exec = (exec_root[(temp >> 26)]);
		break;
	}
	exec();

}

/*
temp <-- (GPR[rs]`31||GPR[rs]`(31..0)) + (GPR[rt]`31||GPR[rt]`(31..0))
if temp`32 != temp`31 then
   SignalException(IntegerOverflow)
else
   GPR[rd] <-- temp
endif
*/
static void
execute_add(void)
{
	unsigned int rs;
	unsigned int rt;
	unsigned int rd;
	unsigned int of = 0;
	// step.2:rd

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	// step.3:alu
	/*
	   asm volatile (
	   "movl %2, %%eax\n\
	   addl %3, %%eax\n\
	   movl %%eax, %0\n\
	   jno .ladd1\n\
	   movl $1, %%eax\n\
	   movl %%eax, %1\n\
	   .ladd1:"
	   :"=m" (rd), "=m"(of)
	   :"m"(rs), "m"(rt)
	   :"%eax");
	 */
	rd = (int) ((int) rs + (int) rt);
	if ((~(rs ^ rt)) & 0x80000000 && (rs ^ rd) & 0x80000000) {
		of = 1;
	}
	//step.4:mem,none.
	//step.5: wb
	if (of) {
		signal_exception(CP0_CAUSE_EC_ALU, cpu_pc_inst, 0);
	} else {
		write_cpu_gpr(i_rd, rd);
	}
}

/*
temp <--(GPR[rs]`31||GPR[rs]`(31..0)) + sign_extend(immediate)
if temp`32 != temp`31 then
   SignalException(IntegerOverflow)
else
   GPR[rt] <-- temp
endif
*/
static void
execute_addi(void)
{
	unsigned int of = 0;
	unsigned int rs;
	unsigned int rt;
	short temp;
	int imm;

	read_cpu_gpr(i_rs, &rs);
	temp = (short) i_imm;
	imm = temp;
	/*
	   asm volatile (
	   "movl %2, %%eax\n\
	   addl %3, %%eax\n\
	   movl %%eax, %0\n\
	   jno .laddi1\n\
	   movl $1, %%eax\n\
	   movl %%eax, %1\n\
	   .laddi1:"
	   :"=m" (rt), "=m"(of)
	   :"m"(rs), "m"(imm)
	   :"%eax");
	 */
	rt = (int) ((int) rs + imm);
	if ((~(rs ^ imm)) & 0x80000000 && (rs ^ rt) & 0x80000000) {
		of = 1;
	}

	if (of) {
		signal_exception(CP0_CAUSE_EC_ALU, cpu_pc_inst, 0);
	} else {
		write_cpu_gpr(i_rt, rt);
	}
}

/*
temp <-- GPR[rs] + GPR[rt]
GPR[rd] <-- temp
*/
static void
execute_addu(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	temp = rs + rt;
	write_cpu_gpr(i_rd, temp);

}

/*
temp <-- GPR[rs] + sign_extend(immediate)
GPR[rt] <-- temp
*/
static void
execute_addiu(void)
{
	unsigned int temp;
	unsigned int rs;

	read_cpu_gpr(i_rs, &rs);
	temp = rs + (unsigned int) (int) (short) i_imm;
	write_cpu_gpr(i_rt, temp);

}

/*
GPR[rd] <-- GPR[rs] and GPR[rt]
*/
static void
execute_and(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	temp = rs & rt;
	write_cpu_gpr(i_rd, temp);

}

/*
GPR[rt] <-- GPR[rs] and zero_extend(immediate)
*/
static void
execute_andi(void)
{
	unsigned int temp;
	unsigned int rs;

	read_cpu_gpr(i_rs, &rs);
	temp = rs & i_imm;
	write_cpu_gpr(i_rt, temp);

}

/*
q  <-- GPR[rs]`(31..0) div GPR[rt]`(31..0)
LO <-- q
r  <-- GPR[rs]`(31..0) mod GPR[rt]`(31..0)
HI <-- r

*/
static void
execute_div(void)
{
	int temphi;
	int templo;
	int rs;
	int rt;

	read_cpu_gpr(i_rs, (unsigned int *) &rs);
	read_cpu_gpr(i_rt, (unsigned int *) &rt);
	if (rt == 0)				// no exception.
		return;
	templo = rs / rt;
	temphi = rs % rt;
	cpuReg.lo = templo;
	cpuReg.hi = temphi;

}

/*
q  <-- (0 || GPR[rs]`(31..0)) div (0 || GPR[rt]`(31..0))
r  <-- (0 || GPR[rs]`(31..0)) mod (0 || GPR[rt]`(31..0))
LO <-- sign_extend(q`(31..0))
HI <-- sign_extend(r`(31..0))
*/
static void
execute_divu(void)
{
	unsigned int temphi;
	unsigned int templo;
	unsigned int rs;
	unsigned int rt;

	read_cpu_gpr(i_rs, (unsigned int *) &rs);
	read_cpu_gpr(i_rt, (unsigned int *) &rt);
	if (rt == 0)				// no exception.
		return;
	templo = rs / rt;
	temphi = rs % rt;
	cpuReg.lo = templo;
	cpuReg.hi = temphi;
}

/*
prod <-- GPR[rs]`(31..0) × GPR[rt]`(31..0)
LO   <-- prod31..0
HI   <-- prod63..32
*/
static void
execute_mult(void)
{
	unsigned int rs;
	unsigned int rt;
	unsigned int hi;
	unsigned int lo;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);

	/* It is easy to mult hi. */
	asm volatile ("movl %2, %%eax\n\
		movl %3, %%ecx\n\
		imull %%ecx\n\
		movl %%edx, %1\n\
		movl %%eax, %0":"=m" (lo), "=m"(hi)
				  :"m"(rs), "m"(rt)
				  :"%eax", "%ecx", "%edx");

	cpuReg.hi = hi;
	cpuReg.lo = lo;
}

/*
prod <-- (0 || GPR[rs]`(31..0)) × (0 || GPR[rt]`(31..0))
LO   <-- prod31..0
HI   <-- prod63..32
*/
static void
execute_multu(void)
{
	unsigned int rs;
	unsigned int rt;
	unsigned int hi;
	unsigned int lo;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);

	/* It is easy to multu hi. */
	asm volatile ("movl %2, %%eax\n\
		movl %3, %%ecx\n\
		mull %%ecx\n\
		movl %%edx, %1\n\
		movl %%eax, %0":"=m" (lo), "=m"(hi)
				  :"m"(rs), "m"(rt)
				  :"%eax", "%ecx", "%edx");
	cpuReg.hi = hi;
	cpuReg.lo = lo;
}

/*
GPR[rd] <-- GPR[rs] nor GPR[rt]
*/
static void
execute_nor(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	temp = ~(rs | rt);
	write_cpu_gpr(i_rd, temp);

}

/*
GPR[rd] <-- GPR[rs] or GPR[rt]
*/
static void
execute_or(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	temp = rs | rt;
	write_cpu_gpr(i_rd, temp);

}

/*
GPR[rt] <-- GPR[rs] or zero_extend(immediate)
*/
static void
execute_ori(void)
{
	unsigned int temp;
	unsigned int rs;

	read_cpu_gpr(i_rs, &rs);
	temp = rs | i_imm;
	write_cpu_gpr(i_rt, temp);

}

/*
s       <-- sa
temp    <-- GPR[rt]`((31-s)..0) || 0`s
GPR[rd] <--temp
*/
static void
execute_sll(void)
{
	unsigned int temp;
	unsigned int rt;

	read_cpu_gpr(i_rt, &rt);
	temp = rt << i_sa;
	write_cpu_gpr(i_rd, temp);

}

/*
s       <-- GPR[rs]4..0
temp    <-- GPR[rt]`((31-s)..0) || 0`s
GPR[rd] <-- temp
*/
static void
execute_sllv(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	temp = rt << (rs & 0x1f);
	write_cpu_gpr(i_rd, temp);

}

/*
s       <-- ra
temp    <-- (GPR[rt]31)`s || GPR[rt]`(31..s)
GPR[rd] <-- temp
*/
static void
execute_sra(void)
{
	unsigned int temp;
	unsigned int rt;

	read_cpu_gpr(i_rt, &rt);
	temp = ((int) rt) >> i_sa;
	write_cpu_gpr(i_rd, temp);

}

/*
s       <-- GPR[rs]4..0
temp    <-- (GPR[rt]31)`s || GPR[rt]`(31..s)
GPR[rd] <-- temp
*/
static void
execute_srav(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	temp = ((int) rt) >> (rs & 0x1f);
	write_cpu_gpr(i_rd, temp);

}

/*
s       <-- ra
temp    <-- 0`s || GPR[rt]`(31..s)
GPR[rd] <-- temp
*/
static void
execute_srl(void)
{
	unsigned int temp;
	unsigned int rt;

	read_cpu_gpr(i_rt, &rt);
	temp = rt >> i_sa;
	write_cpu_gpr(i_rd, temp);

}

/*
s       <-- GPR[rs]`(4..0)
temp    <-- 0`s || GPR[rt]`(31..s)
GPR[rd] <-- temp
*/
static void
execute_srlv(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	temp = rt >> (rs & 0x1f);
	write_cpu_gpr(i_rd, temp);

}

/*
temp <-- (GPR[rs]`31||GPR[rs]`(31..0)) - (GPR[rt]`31||GPR[rt]`(31..0))
if temp`32 != temp`31 then
   SignalException(IntegerOverflow)
else
   GPR[rd] <-- temp`(31..0)
endif
*/
static void
execute_sub(void)
{
	unsigned int rs;
	unsigned int rt;
	unsigned int rd;
	unsigned int of = 0;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	asm volatile ("movl %2, %%eax\n\
		subl %3, %%eax\n\
		movl %%eax, %0\n\
		jno .lsub1\n\
		movl $1, %%eax\n\
		movl %%eax, %1\n\
		.lsub1:":"=m" (rd), "=m"(of)
				  :"m"(rs), "m"(rt)
				  :"%eax");

	if (of) {
		signal_exception(CP0_CAUSE_EC_ALU, cpu_pc_inst, 0);
	} else {
		write_cpu_gpr(i_rd, rd);
	}
}

/*
temp <-- GPR[rs] - GPR[rt]
GPR[rd] <-- temp
*/
static void
execute_subu(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	temp = rs - rt;
	write_cpu_gpr(i_rd, temp);

}

/*
GPR[rd] <-- GPR[rs] xor GPR[rt]
*/
static void
execute_xor(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	temp = rs ^ rt;
	write_cpu_gpr(i_rd, temp);

}

/*
GPR[rt] <-- GPR[rs] xor zero_extend(immediate)
*/
static void
execute_xori(void)
{
	unsigned int temp;
	unsigned int rs;

	read_cpu_gpr(i_rs, &rs);
	temp = rs ^ i_imm;
	write_cpu_gpr(i_rt, temp);

}

/*
GPR[rt] <-- immediate || 0`16
*/
static void
execute_lui(void)
{
	unsigned int temp;

	temp = i_imm << 16;
	write_cpu_gpr(i_rt, temp);

}

/*
if GPR[rs] < GPR[rt] then
   GPR[rd] <-- 0`(GPRLEN-1) || 1
else
   GPR[rd] <-- 0`GPRLEN
endif
*/
static void
execute_slt(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	temp = ((int) rs < (int) rt) ? 1 : 0;
	write_cpu_gpr(i_rd, temp);

}

/*
if GPR[rs] < sign_extend(immediate) then
   GPR[rt] <-- 0`(GPRLEN-1)|| 1
else
   GPR[rt] <-- 0`GPRLEN
endif
*/
static void
execute_slti(void)
{
	unsigned int temp;
	short imm;
	unsigned int rs;

	imm = i_imm;
	read_cpu_gpr(i_rs, &rs);

	temp = (((int) rs) < ((int)imm)) ? 1 : 0;
	write_cpu_gpr(i_rt, temp);

}

/*
if (0 || GPR[rs]) < (0 || GPR[rt]) then
   GPR[rd] <-- 0`(GPRLEN-1) || 1
else
   GPR[rd] <-- 0`GPRLEN
endif

*/
static void
execute_sltu(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	temp = (rs < rt) ? 1 : 0;
	write_cpu_gpr(i_rd, temp);

}

/*
if (0 || GPR[rs]) < (0 || sign_extend(immediate)) then
   GPR[rt] <-- 0`(GPRLEN-1) || 1
else
   GPR[rt] <-- 0`GPRLEN
endif
*/
static void
execute_sltiu(void)
{
	unsigned int temp;
	unsigned int rs;

	read_cpu_gpr(i_rs, &rs);
	/*temp = (rs < i_imm) ? 1 : 0;*/
	temp = (rs < (unsigned int)((int)((short)i_imm))) ? 1 : 0;
	write_cpu_gpr(i_rt, temp);

}

/*
I:  
target_offset <-- sign_extend(offset || 0`2)
    condition <-- (GPR[rs] = GPR[rt])

I+1:
if condition then
    PC <-- PC + target_offset
endif
*/
static void
execute_beq(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;
	unsigned int pc;
	short offset;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	pc = cpu_pc;

	offset = i_imm;
	temp = (int) pc + (((int) offset) << 2);
	if (rs == rt) {
		// TODO : maybe execute the delay slot inst.
		cpu_pc = temp;
	}

}

/*
I:  
target_offset <-- sign_extend(offset || 0`2)
    condition <-- GPR[rs] >= 0`GPRLEN

I+1:
if condition then
    PC <-- PC + target_offset
endif
*/
static void
execute_bgez(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int pc;
	short offset;

	read_cpu_gpr(i_rs, &rs);
	pc = cpu_pc;
	offset = i_imm;
	temp = (int) pc + (((int) offset) << 2);
	if ((int) rs >= 0) {
		// TODO : maybe execute the delay slot inst.
		cpu_pc = temp;
	}

}

/*
I:  
target_offset <-- sign_extend(offset || 0`2)
    condition <-- GPR[rs] >= 0`GPRLEN
    GPR[31] <-- PC + 8

I+1:
if condition then
    PC <-- PC + target_offset
endif
*/
static void
execute_bgezal(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int pc;
	short offset;

	read_cpu_gpr(i_rs, &rs);
	pc = cpu_pc;
	offset = i_imm;
	temp = (int) pc + (((int) offset) << 2);
	if ((int) rs >= 0) {
		// TODO : maybe execute the delay slot inst.
		cpuReg.gpr[GPR_RA] = pc + 4;
		cpu_pc = temp;

	}
}

/*
I:  
target_offset <-- sign_extend(offset || 0`2)
    condition <-- GPR[rs] > 0`GPRLEN

I+1:
if condition then
    PC <-- PC + target_offset
endif
*/
static void
execute_bgtz(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int pc;
	short offset;

	read_cpu_gpr(i_rs, &rs);
	pc = cpu_pc;
	offset = i_imm;
	temp = (int) pc + (((int) offset) << 2);
	if ((int) rs > 0) {
		// TODO : maybe execute the delay slot inst.
		cpu_pc = temp;
	}
}

/*
I:  
target_offset <-- sign_extend(offset || 0`2)
    condition <-- GPR[rs] <= 0`GPRLEN

I+1:
if condition then
    PC <-- PC + target_offset
endif
*/
static void
execute_blez(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int pc;
	short offset;

	read_cpu_gpr(i_rs, &rs);
	pc = cpu_pc;
	offset = i_imm;
	temp = (int) pc + (((int) offset) << 2);
	if ((int) rs <= 0) {
		// TODO : maybe execute the delay slot inst.
		cpu_pc = temp;
	}
}

/*
I:  
target_offset <-- sign_extend(offset || 0`2)
    condition <-- GPR[rs] < 0`GPRLEN
    GPR[31] <-- PC + 8

I+1:
if condition then
    PC <-- PC + target_offset
endif
*/
static void
execute_bltzal(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int pc;
	short offset;

	read_cpu_gpr(i_rs, &rs);
	pc = cpu_pc;
	offset = i_imm;
	temp = (int) pc + (((int) offset) << 2);
	if ((int) rs < 0) {
		// TODO : maybe execute the delay slot inst.
		cpuReg.gpr[GPR_RA] = pc + 4;
		cpu_pc = temp;
	}
}

/*
I:  
target_offset <-- sign_extend(offset || 0`2)
    condition <-- GPR[rs] < 0`GPRLEN

I+1:
if condition then
    PC <-- PC + target_offset
endif
*/
static void
execute_bltz(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int pc;
	short offset;

	read_cpu_gpr(i_rs, &rs);
	pc = cpu_pc;
	offset = i_imm;
	temp = (int) pc + (((int) offset) << 2);
	if ((int) rs < 0) {
		// TODO : maybe execute the delay slot inst.
		cpu_pc = temp;

	}
}

/*
I:  
target_offset <-- sign_extend(offset || 0`2)
    condition <-- (GPR[rs] != GPR[rt])

I+1:
if condition then
    PC <-- PC + target_offset
endif
*/
static void
execute_bne(void)
{
	unsigned int temp;
	unsigned int rs;
	unsigned int rt;
	unsigned int pc;
	short offset;

	read_cpu_gpr(i_rs, &rs);
	read_cpu_gpr(i_rt, &rt);
	pc = cpu_pc;

	offset = i_imm;
	temp = (int) pc + (((int) offset) << 2);
	if (rs != rt) {
		// TODO : maybe execute the delay slot inst.
		cpu_pc = temp;
	}
}

/*
I:

I+1:
PC <-- PC`((GPRLEN-1)..28) || instr_index || 0`2
*/
static void
execute_j(void)
{
	unsigned int pc;

	pc = cpu_pc;
	pc = (pc & 0xf0000000) | (i_jaddr << 2);
	cpu_pc = pc;
}

/*
I:
GPR[31] <-- PC + 8
I+1:
PC <-- PC`((GPRLEN-1)..28) || instr_index || 0`2
*/
static void
execute_jal(void)
{
	unsigned int pc;
	unsigned int temp;

	pc = cpu_pc;
	temp = pc + 4;
	pc = (pc & 0xf0000000) | (i_jaddr << 2);

	write_cpu_gpr(GPR_RA, temp);
	cpu_pc = pc;

}

/*
I: 
temp <-- GPR[rs]
GPR[rd] <-- PC + 8
   
I+1:
if temp`(1..0)
	SignalException(AddressException)
else
	PC <-- temp`((GPRLEN-1)..2) || 0`2
endif
*/
static void
execute_jalr(void)
{
	unsigned int pc;
	unsigned int rs;
	unsigned int temp;

	read_cpu_gpr(i_rs, &rs);
	pc = cpu_pc;
	temp = pc + 4;

	write_cpu_gpr(i_rd, temp);
	if (rs & 3) {
		// pc must align to 00 ?
		signal_exception(CP0_CAUSE_EC_ADEL, cpu_pc_inst, rs);
	}
	// assume that pc must be *00 ??
	//cpu_pc = rs & 0xFFFFFFFC;
	cpu_pc = rs;
}

/*
I: 
temp <-- GPR[rs]
   
I+1:
if temp`(1..0)
	SignalException(AddressException)
else
	PC <-- temp`((GPRLEN-1)..2) || 0`2
endif
*/
static void
execute_jr(void)
{
	unsigned int rs;

	read_cpu_gpr(i_rs, &rs);
	if (rs & 3) {
		// pc must align to 00 ?
		signal_exception(CP0_CAUSE_EC_ADEL, cpu_pc_inst, rs);
	}
	//cpu_pc = rs & 0xFFFFFFFC;
	cpu_pc = rs;
}

/*
Addr <-- sign_extend(offset) + GPR[base]
membyte <-- LoadMemory (BYTE, Addr)
GPR[rt] <-- sign_extend(membyte)
*/
static void
execute_lb(void)
{
	unsigned int addr;
	unsigned int rs;
	unsigned int exccode;
	char temp;

	read_cpu_gpr(i_rs, &rs);
	addr = ((int) rs) + (short) i_imm;

	exccode = address_read_8(addr, (unsigned char *) &temp);

	if (exccode) {
		signal_exception(exccode, cpu_pc_inst, 0);
	} else {
		write_cpu_gpr(i_rt, (int) temp);
	}
}

/*
Addr <-- sign_extend(offset) + GPR[base]
membyte <-- LoadMemory (BYTE, Addr)
GPR[rt] <-- zero_extend(membyte)
*/
static void
execute_lbu(void)
{
	unsigned int addr;
	unsigned int rs;
	unsigned int exccode;
	unsigned char temp;

	read_cpu_gpr(i_rs, &rs);
	addr = ((int) rs) + (short) i_imm;

	exccode = address_read_8(addr, &temp);

	if (exccode) {
		signal_exception(exccode, cpu_pc_inst, 0);
	} else {
		write_cpu_gpr(i_rt, (unsigned int) temp);
	}
}

/*
Addr <-- sign_extend(offset) + GPR[base]
memhalf <-- LoadMemory (HALFWORD, Addr)
if Addr`0 != 0 then
    SignalException(AddressError)
else
    GPR[rt] <-- sign_extend(memhalf)
endif
*/
static void
execute_lh(void)
{
	unsigned int addr;
	unsigned int rs;
	unsigned int exccode;
	short temp;

	read_cpu_gpr(i_rs, &rs);
	addr = ((int) rs) + (short) i_imm;

	exccode = address_read_16(addr, (unsigned short *) &temp);

	if (exccode) {
		signal_exception(exccode, cpu_pc_inst, 0);
	} else {
		write_cpu_gpr(i_rt, (int) temp);
	}
}

/*
Addr <-- sign_extend(offset) + GPR[base]
memhalf <-- LoadMemory (HALFWORD, Addr)
if Addr`0 != 0 then
    SignalException(AddressError)
else
    GPR[rt] <-- zero_extend(memhalf)
endif
*/
static void
execute_lhu(void)
{
	unsigned int addr;
	unsigned int rs;
	unsigned int exccode;
	unsigned short temp;

	read_cpu_gpr(i_rs, &rs);
	addr = ((int) rs) + (short) i_imm;

	exccode = address_read_16(addr, &temp);

	if (exccode) {
		signal_exception(exccode, cpu_pc_inst, 0);
	} else {
		write_cpu_gpr(i_rt, (unsigned int) temp);
	}
}

/*
Addr <-- sign_extend(offset) + GPR[base]
memword <-- LoadMemory (WORD, Addr)
if Addr`(1..0) != 0 then
    SignalException(AddressError)
else
    GPR[rt] <-- memword
endif
*/
static void
execute_lw(void)
{
	unsigned int addr;
	unsigned int rs;
	unsigned int exccode;
	unsigned int temp;

	read_cpu_gpr(i_rs, &rs);
	addr = ((int) rs) + (short) i_imm;

	exccode = address_read_32(addr, &temp);

	if (exccode) {
		signal_exception(exccode, cpu_pc_inst, 0);
	} else {
		write_cpu_gpr(i_rt, temp);
	}
}

/*

*/
static void
execute_sb(void)
{
	unsigned int addr;
	unsigned int rs;
	unsigned int exccode;
	unsigned char data;
	unsigned int temp;

	read_cpu_gpr(i_rs, &rs);
	addr = ((int) rs) + ((short) i_imm);

	read_cpu_gpr(i_rt, &temp);
	data = (unsigned char) temp;

	exccode = address_write_8(addr, data);

	if (exccode) {
		signal_exception(exccode, cpu_pc_inst, 0);
	}
}

/*

*/
static void
execute_sh(void)
{
	unsigned int addr;
	unsigned int rs;
	unsigned int exccode;
	unsigned short data;
	unsigned int temp;

	read_cpu_gpr(i_rs, &rs);
	addr = ((int) rs) + (short) i_imm;

	read_cpu_gpr(i_rt, &temp);
	data = (unsigned short) temp;

	exccode = address_write_16(addr, data);

	if (exccode) {
		signal_exception(exccode, cpu_pc_inst, 0);
	}
}

/*

*/
static void
execute_sw(void)
{
	unsigned int addr;
	unsigned int rs;
	unsigned int exccode;
	unsigned int data;

	read_cpu_gpr(i_rs, &rs);
	addr = ((int) rs) + (short) i_imm;

	read_cpu_gpr(i_rt, &data);

	exccode = address_write_32(addr, data);

	if (exccode) {
		signal_exception(exccode, cpu_pc_inst, 0);
	}
}

/*
SignalException(Breakpoint)
*/
static void
execute_break(void)
{

	signal_exception(CP0_CAUSE_EC_BP, cpu_pc_inst, 0);
}

/*
SignalException(SystemCall)
*/
static void
execute_syscall(void)
{
	signal_exception(CP0_CAUSE_EC_SC, cpu_pc_inst, 0);
}

/*
GPR[rd] <-- HI
*/
static void
execute_mfhi(void)
{
	unsigned int temp;

	temp = cpuReg.hi;
	write_cpu_gpr(i_rd, temp);

}

/*
GPR[rd] <-- LO
*/
static void
execute_mflo(void)
{
	unsigned int temp;

	temp = cpuReg.lo;
	write_cpu_gpr(i_rd, temp);

}

/*
HI <-- GPR[rs]
*/
static void
execute_mthi(void)
{
	unsigned int temp;

	read_cpu_gpr(i_rs, &temp);
	cpuReg.hi = temp;
	wreg = 32;
}

/*
LO <-- GPR[rs]
*/
static void
execute_mtlo(void)
{
	unsigned int temp;

	read_cpu_gpr(i_rs, &temp);
	cpuReg.lo = temp;
	wreg = 33;
}

/*
data <--CPR[rd]
GPR[rt] <-- data
*/
static void
execute_mfc0(void)
{
	unsigned int temp;

	read_cp0_reg(i_rd, &temp);
	write_cpu_gpr(i_rt, temp);

}

/*
data <-- GPR[rt]
CPR[rd] <-- data

*/
static void
execute_mtc0(void)
{
	unsigned int temp;
	wcp0 = i_rd;

	read_cpu_gpr(i_rt, &temp);
	write_cp0_reg(i_rd, temp);
}

/*
PC <-- CPR[epc]
CPR[sr]`0 = 1;
*/
static void
execute_eretx(void)
{
	unsigned int pc_ret;

	pc_ret = exception_return();
	cpu_pc = pc_ret & 0xFFFFFFFC;
}

static void
execute_null(void)
{
	signal_exception(CP0_CAUSE_EC_RI, cpu_pc_inst, 0);
}

/***********************************************************************/

static void
read_cpu_gpr(unsigned int id, unsigned int *data)
{
	//printf("\n--read reg : $%d, data : %x\n", id, cpuReg.gpr[id]);
	if (id) {
		*data = cpuReg.gpr[id];
	} else {
		*data = 0;
	}
}

static void
write_cpu_gpr(unsigned int id, unsigned int data)
{
	//printf("\n--write reg : $%d, data : %x\n", id, data);
	if (id) {
		cpuReg.gpr[id] = data;
	}
	wreg = id;
}

static unsigned int
fetch_inst(void)
{
	unsigned int exccode;

	/* store the pc of current instruction to cpu_pc_inst. */
	cpu_pc_inst = cpu_pc;
	exccode = address_read_32(cpu_pc, &inst);

	/* here add the pc immediately. */
	cpu_pc += 4;

	i_rs = INST_SPLIT_25_21(inst);
	i_rt = INST_SPLIT_20_16(inst);
	i_rd = INST_SPLIT_15_11(inst);
	i_imm = INST_SPLIT_IMM(inst);
	i_jaddr = INST_SPLIT_JADDR(inst);
	i_sa = INST_SPLIT_10_6(inst);
	return exccode;
}

void
show_cpu_regs(void)
{
	unsigned int next_inst;
	printf("$00/00:%08x   ", cpuReg.gpr[0]);
	printf("$01/at:%08x%c  ", cpuReg.gpr[1], wreg == 1 ? '!' : ' ');
	printf("$02/v0:%08x%c  ", cpuReg.gpr[2], wreg == 2 ? '!' : ' ');
	printf("$03/v1:%08x%c\n", cpuReg.gpr[3], wreg == 3 ? '!' : ' ');
	printf("$04/a0:%08x%c  ", cpuReg.gpr[4], wreg == 4 ? '!' : ' ');
	printf("$05/a1:%08x%c  ", cpuReg.gpr[5], wreg == 5 ? '!' : ' ');
	printf("$06/a2:%08x%c  ", cpuReg.gpr[6], wreg == 6 ? '!' : ' ');
	printf("$07/a3:%08x%c\n", cpuReg.gpr[7], wreg == 7 ? '!' : ' ');
	printf("$08/t0:%08x%c  ", cpuReg.gpr[8], wreg == 8 ? '!' : ' ');
	printf("$09/t1:%08x%c  ", cpuReg.gpr[9], wreg == 9 ? '!' : ' ');
	printf("$10/t2:%08x%c  ", cpuReg.gpr[10], wreg == 10 ? '!' : ' ');
	printf("$11/t3:%08x%c\n", cpuReg.gpr[11], wreg == 11 ? '!' : ' ');
	printf("$12/t4:%08x%c  ", cpuReg.gpr[12], wreg == 12 ? '!' : ' ');
	printf("$13/t5:%08x%c  ", cpuReg.gpr[13], wreg == 13 ? '!' : ' ');
	printf("$14/t6:%08x%c  ", cpuReg.gpr[14], wreg == 14 ? '!' : ' ');
	printf("$15/t7:%08x%c\n", cpuReg.gpr[15], wreg == 15 ? '!' : ' ');
	printf("$16/s0:%08x%c  ", cpuReg.gpr[16], wreg == 16 ? '!' : ' ');
	printf("$17/s1:%08x%c  ", cpuReg.gpr[17], wreg == 17 ? '!' : ' ');
	printf("$18/s2:%08x%c  ", cpuReg.gpr[18], wreg == 18 ? '!' : ' ');
	printf("$19/s3:%08x%c\n", cpuReg.gpr[19], wreg == 19 ? '!' : ' ');
	printf("$20/s4:%08x%c  ", cpuReg.gpr[20], wreg == 20 ? '!' : ' ');
	printf("$21/s5:%08x%c  ", cpuReg.gpr[21], wreg == 21 ? '!' : ' ');
	printf("$22/s6:%08x%c  ", cpuReg.gpr[22], wreg == 22 ? '!' : ' ');
	printf("$23/s7:%08x%c\n", cpuReg.gpr[23], wreg == 23 ? '!' : ' ');
	printf("$24/t8:%08x%c  ", cpuReg.gpr[24], wreg == 24 ? '!' : ' ');
	printf("$25/t9:%08x%c  ", cpuReg.gpr[25], wreg == 25 ? '!' : ' ');
	printf("$26/k0:%08x%c  ", cpuReg.gpr[26], wreg == 26 ? '!' : ' ');
	printf("$27/k1:%08x%c\n", cpuReg.gpr[27], wreg == 27 ? '!' : ' ');
	printf("$28/gp:%08x%c  ", cpuReg.gpr[28], wreg == 28 ? '!' : ' ');
	printf("$29/sp:%08x%c  ", cpuReg.gpr[29], wreg == 29 ? '!' : ' ');
	printf("$30/fp:%08x%c  ", cpuReg.gpr[30], wreg == 30 ? '!' : ' ');
	printf("$31/ra:%08x%c\n", cpuReg.gpr[31], wreg == 31 ? '!' : ' ');

	printf("$hi:%08x%c  ", cpuReg.hi, wreg == 32 ? '!' : ' ');
	printf("$lo:%08x%c  ", cpuReg.lo, wreg == 33 ? '!' : ' ');
	printf("PC:%08x\n", cpu_pc);
	if (address_read_32(cpu_pc, &next_inst)) {
		printf("Bad addressing!!\n");
	} else {
		disassemble(next_inst);
	}
}

/*
void
show_cpu_written_reg(void)
{
	unsigned int next_inst;
	if(wreg && wreg < 32){
		printf("Written: $%d:%08x\n", wreg, cpuReg.gpr[wreg]);
	}else if(wreg == 32){
		printf("$hi:%08x\n", cpuReg.hi);
	}else if(wreg == 33){
		printf("$lo:%08x\n", cpuReg.lo);
	}
	
	printf("PC:[%08x] : ", cpu_pc);
	if (address_read_32(cpu_pc, &next_inst)) {
		printf("Bad addressing!!\n");
	} else {
		disassemble(next_inst);
	}
}
*/
