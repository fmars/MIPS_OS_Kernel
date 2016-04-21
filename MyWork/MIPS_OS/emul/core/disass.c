#include <stdio.h>
#include "disass.h"
#include "cpu.h"

/*extern int ADDRESS_BIGENDIAN;*/


static unsigned int g_inst;
static unsigned int i_rs = 0;	// also 'base'
static unsigned int i_rt = 0;
static unsigned int i_rd = 0;
static unsigned int i_imm = 0;	// also 'offset'
static unsigned int i_jaddr = 0;
static unsigned int i_sa = 0;

static void show_info_add(void);
static void show_info_addi(void);
static void show_info_addu(void);
static void show_info_addiu(void);

static void show_info_and(void);
static void show_info_andi(void);

static void show_info_div(void);
static void show_info_divu(void);

static void show_info_mult(void);
static void show_info_multu(void);

static void show_info_nor(void);

static void show_info_or(void);
static void show_info_ori(void);

static void show_info_sll(void);
static void show_info_sllv(void);
static void show_info_sra(void);
static void show_info_srav(void);
static void show_info_srl(void);
static void show_info_srlv(void);

static void show_info_sub(void);
static void show_info_subu(void);

static void show_info_xor(void);
static void show_info_xori(void);

static void show_info_lui(void);

static void show_info_slt(void);
static void show_info_slti(void);
static void show_info_sltu(void);
static void show_info_sltiu(void);

static void show_info_beq(void);
static void show_info_bgez(void);
static void show_info_bgezal(void);
static void show_info_bgtz(void);
static void show_info_blez(void);
static void show_info_bltz(void);
static void show_info_bltzal(void);
static void show_info_bne(void);

static void show_info_j(void);
static void show_info_jal(void);
static void show_info_jalr(void);
static void show_info_jr(void);

static void show_info_lb(void);
static void show_info_lbu(void);
static void show_info_lh(void);
static void show_info_lhu(void);
static void show_info_lw(void);

static void show_info_sb(void);
static void show_info_sh(void);
static void show_info_sw(void);

static void show_info_break(void);
static void show_info_syscall(void);

static void show_info_mfhi(void);
static void show_info_mflo(void);
static void show_info_mthi(void);
static void show_info_mtlo(void);

static void show_info_mfc0(void);
static void show_info_mtc0(void);

static void show_info_eretx(void);


void
disassemble(unsigned int inst,int *flag)
{
	g_inst = inst;


	i_rs = INST_SPLIT_25_21(inst);
	i_rt = INST_SPLIT_20_16(inst);
	i_rd = INST_SPLIT_15_11(inst);
	i_imm = INST_SPLIT_IMM(inst);
	i_jaddr = INST_SPLIT_JADDR(inst);
	i_sa = INST_SPLIT_10_6(inst);

	switch (inst & INST_MASK_31_26) {
	case INST_MASK_31_26_x00:
		switch (inst & INST_MASK_5_0) {
		case INST_MASK_5_0_ADD:
			if (INST_CHECK(inst, INST_MASK_ADD, INST_CHECK_ADD))
				show_info_add();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_ADDU:
			if (INST_CHECK(inst, INST_MASK_ADDU, INST_CHECK_ADDU))
				show_info_addu();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_AND:
			if (INST_CHECK(inst, INST_MASK_AND, INST_CHECK_AND))
				show_info_and();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_DIV:
			if (INST_CHECK(inst, INST_MASK_DIV, INST_CHECK_DIV))
				show_info_div();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_DIVU:
			if (INST_CHECK(inst, INST_MASK_DIVU, INST_CHECK_DIVU))
				show_info_divu();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_MULT:
			if (INST_CHECK(inst, INST_MASK_MULT, INST_CHECK_MULT))
				show_info_mult();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_MULTU:
			if (INST_CHECK(inst, INST_MASK_MULTU, INST_CHECK_MULTU))
				show_info_multu();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_NOR:
			if (INST_CHECK(inst, INST_MASK_NOR, INST_CHECK_NOR))
				show_info_nor();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_OR:
			if (INST_CHECK(inst, INST_MASK_OR, INST_CHECK_OR))
				show_info_or();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_SLL:
			if (INST_CHECK(inst, INST_MASK_SLL, INST_CHECK_SLL))
				show_info_sll();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_SLLV:
			if (INST_CHECK(inst, INST_MASK_SLLV, INST_CHECK_SLLV))
				show_info_sllv();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_SRA:
			if (INST_CHECK(inst, INST_MASK_SRA, INST_CHECK_SRA))
				show_info_sra();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_SRAV:
			if (INST_CHECK(inst, INST_MASK_SRAV, INST_CHECK_SRAV))
				show_info_srav();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_SRL:
			if (INST_CHECK(inst, INST_MASK_SRL, INST_CHECK_SRL))
				show_info_srl();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_SRLV:
			if (INST_CHECK(inst, INST_MASK_SRLV, INST_CHECK_SRLV))
				show_info_srlv();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_SUB:
			if (INST_CHECK(inst, INST_MASK_SUB, INST_CHECK_SUB))
				show_info_sub();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_SUBU:
			if (INST_CHECK(inst, INST_MASK_SUBU, INST_CHECK_SUBU))
				show_info_subu();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_XOR:
			if (INST_CHECK(inst, INST_MASK_XOR, INST_CHECK_XOR))
				show_info_xor();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_SLT:
			if (INST_CHECK(inst, INST_MASK_SLT, INST_CHECK_SLT))
				show_info_slt();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_SLTU:
			if (INST_CHECK(inst, INST_MASK_SLTU, INST_CHECK_SLTU))
				show_info_sltu();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;

		case INST_MASK_5_0_JALR:  // pds------------------------------------------------
			if (INST_CHECK(inst, INST_MASK_JALR, INST_CHECK_JALR))
				show_info_jalr();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;

		case INST_MASK_5_0_JR:  // pds -------------------------------------------------
			if (INST_CHECK(inst, INST_MASK_JR, INST_CHECK_JR))
				show_info_jr();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_MFHI:
			if (INST_CHECK(inst, INST_MASK_MFHI, INST_CHECK_MFHI))
				show_info_mfhi();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_MFLO:
			if (INST_CHECK(inst, INST_MASK_MFLO, INST_CHECK_MFLO))
				show_info_mflo();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;

		case INST_MASK_5_0_MTHI:  // pds--------------------------------------------------
			if (INST_CHECK(inst, INST_MASK_MTHI, INST_CHECK_MTHI))
				show_info_mthi();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_MTLO:  // pds---------------------------------------------------
			if (INST_CHECK(inst, INST_MASK_MTLO, INST_CHECK_MTLO))
				show_info_mtlo();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;

		case INST_MASK_5_0_SYSCALL:
			if (INST_CHECK(inst, INST_MASK_SYSCALL, INST_CHECK_SYSCALL))
				show_info_syscall();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_5_0_BREAK:
			if (INST_CHECK(inst, INST_MASK_BREAK, INST_CHECK_BREAK))
				show_info_break();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		default:
			{printf("unknown instruction\n"); *flag=1;}
			break;
		}
		break;
	case INST_MASK_31_26_x01:
		switch (inst & INST_MASK_20_16) {
		case INST_MASK_20_16_BGEZ:
			if (INST_CHECK(inst, INST_MASK_BGEZ, INST_CHECK_BGEZ))
				show_info_bgez();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;

		case INST_MASK_20_16_BGEZAL:  // pds-----------------------------------------
			if (INST_CHECK(inst, INST_MASK_BGEZAL, INST_CHECK_BGEZAL))
				show_info_bgezal();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_20_16_BLTZAL:  // pds -----------------------------------------
			if (INST_CHECK(inst, INST_MASK_BLTZAL, INST_CHECK_BLTZAL))
				show_info_bltzal();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;

		case INST_MASK_20_16_BLTZ:
			if (INST_CHECK(inst, INST_MASK_BLTZ, INST_CHECK_BLTZ))
				show_info_bltz();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		default:
			{printf("unknown instruction\n"); *flag=1;}
			break;
		}
		break;
	case INST_MASK_31_26_x10:
		switch (inst & INST_MASK_25_21) {
		case INST_MASK_25_21_MFC0:
			if (INST_CHECK(inst, INST_MASK_MFC0, INST_CHECK_MFC0))
				show_info_mfc0();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_25_21_MTC0:
			if (INST_CHECK(inst, INST_MASK_MTC0, INST_CHECK_MTC0))
				show_info_mtc0();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		case INST_MASK_25_21_ERETX:
			if (INST_CHECK(inst, INST_MASK_ERETX, INST_CHECK_ERETX))
				show_info_eretx();
			else
				{printf("unknown instruction\n"); *flag=1;}
			break;
		default:
			{printf("unknown instruction\n"); *flag=1;}
			break;
		}
		break;
	case INST_MASK_31_26_ADDI:
		if (INST_CHECK(inst, INST_MASK_ADDI, INST_CHECK_ADDI))
			show_info_addi();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_ADDIU:
		if (INST_CHECK(inst, INST_MASK_ADDIU, INST_CHECK_ADDIU))
			show_info_addiu();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_ANDI:
		if (INST_CHECK(inst, INST_MASK_ANDI, INST_CHECK_ANDI))
			show_info_andi();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_ORI:
		if (INST_CHECK(inst, INST_MASK_ORI, INST_CHECK_ORI))
			show_info_ori();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_XORI:
		if (INST_CHECK(inst, INST_MASK_XORI, INST_CHECK_XORI))
			show_info_xori();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_LUI:
		if (INST_CHECK(inst, INST_MASK_LUI, INST_CHECK_LUI))
			show_info_lui();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_SLTI:
		if (INST_CHECK(inst, INST_MASK_SLTI, INST_CHECK_SLTI))
			show_info_slti();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_SLTIU:
		if (INST_CHECK(inst, INST_MASK_SLTIU, INST_CHECK_SLTIU))
			show_info_sltiu();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_BEQ:
		if (INST_CHECK(inst, INST_MASK_BEQ, INST_CHECK_BEQ))
			show_info_beq();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_BGTZ:
		if (INST_CHECK(inst, INST_MASK_BGTZ, INST_CHECK_BGTZ))
			show_info_bgtz();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_BLEZ:
		if (INST_CHECK(inst, INST_MASK_BLEZ, INST_CHECK_BLEZ))
			show_info_blez();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_BNE:
		if (INST_CHECK(inst, INST_MASK_BNE, INST_CHECK_BNE))
			show_info_bne();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_J:
		if (INST_CHECK(inst, INST_MASK_J, INST_CHECK_J))
			show_info_j();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_JAL:
		if (INST_CHECK(inst, INST_MASK_JAL, INST_CHECK_JAL))
			show_info_jal();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;

	case INST_MASK_31_26_LB:  // pds---------------------------------------------------
		if (INST_CHECK(inst, INST_MASK_LB, INST_CHECK_LB))
			show_info_lb();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;

	case INST_MASK_31_26_LBU:
		if (INST_CHECK(inst, INST_MASK_LBU, INST_CHECK_LBU))
			show_info_lbu();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;

	case INST_MASK_31_26_LH:  // pds--------------------------------------------
		if (INST_CHECK(inst, INST_MASK_LH, INST_CHECK_LH))
			show_info_lh();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;

	case INST_MASK_31_26_LHU:
		if (INST_CHECK(inst, INST_MASK_LHU, INST_CHECK_LHU))
			show_info_lhu();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_LW:
		if (INST_CHECK(inst, INST_MASK_LW, INST_CHECK_LW))
			show_info_lw();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_SB:
		if (INST_CHECK(inst, INST_MASK_SB, INST_CHECK_SB))
			show_info_sb();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_SH:
		if (INST_CHECK(inst, INST_MASK_SH, INST_CHECK_SH))
			show_info_sh();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	case INST_MASK_31_26_SW:
		if (INST_CHECK(inst, INST_MASK_SW, INST_CHECK_SW))
			show_info_sw();
		else
			{printf("unknown instruction\n"); *flag=1;}
		break;
	default:
		{printf("unknown instruction\n"); *flag=1;}
		break;
	}  
}

static void
show_info_add()
{
	printf("add $%d, $%d, $%d", i_rd, i_rs, i_rt);
	printf(", ( $%d = $%d + $%d )\n", i_rd, i_rs, i_rt);
}

static void
show_info_addi()
{
	printf("addi $%d, $%d, %d", i_rt, i_rs, (short) i_imm);
	printf(", ( $%d = $%d + %d )\n", i_rt, i_rs, (short) i_imm);
}

static void
show_info_addu()
{
	printf("addu $%d, $%d, $%d", i_rd, i_rs, i_rt);
	printf(", ( $%d = $%d + $%d )\n", i_rd, i_rs, i_rt);
}

static void
show_info_addiu()
{
	printf("addiu $%d, $%d, %u", i_rt, i_rs, (int)(short)i_imm);
	printf(", ( $%d = $%d + %u )\n", i_rt, i_rs, (int)(short)i_imm);
}

static void
show_info_and()
{
	printf("and $%d, $%d, $%d", i_rd, i_rs, i_rt);
	printf(", ( $%d = $%d & $%d )\n", i_rd, i_rs, i_rt);
}

static void
show_info_andi()
{
	printf("andi $%d, $%d, %d", i_rt, i_rs, (short) i_imm);
	printf(", ( $%d = $%d & 0x%x )\n", i_rt, i_rs, (short) i_imm);
}

static void
show_info_div()
{
	printf("div $%d, $%d", i_rs, i_rt);
	printf(", ( $%d / $%d )\n", i_rs, i_rt);
}

static void
show_info_divu()
{
	printf("divu $%d, $%d", i_rs, i_rt);
	printf(", ( $%d / $%d )\n", i_rs, i_rt);
}

static void
show_info_mult()
{
	printf("mult $%d, $%d", i_rs, i_rt);
	printf(", ( $%d * $%d )\n", i_rs, i_rt);
}

static void
show_info_multu()
{
	printf("multu $%d, $%d", i_rs, i_rt);
	printf(", ( $%d * $%d )\n", i_rs, i_rt);
}

static void
show_info_nor()
{
	printf("nor $%d, $%d, $%d", i_rd, i_rs, i_rt);
	printf(", ( $%d = ^ ( $%d | $%d ) )\n", i_rd, i_rs, i_rt);
}

static void
show_info_or()
{
	printf("or $%d, $%d, $%d", i_rd, i_rs, i_rt);
	printf(", ( $%d = $%d | $%d )\n", i_rd, i_rs, i_rt);
}

static void
show_info_ori()
{
	printf("ori $%d, $%d, %d", i_rt, i_rs, (short) i_imm);
	printf(", ( $%d = $%d | 0x%x )\n", i_rd, i_rs, (int) (short) i_imm);
}

static void
show_info_sll()
{
	printf("sll $%d, $%d, %d", i_rd, i_rt, i_sa);
	printf(", ( $%d = $%d << %d )\n", i_rd, i_rt, i_sa);
}

static void
show_info_sllv()
{
	printf("sllv $%d, $%d, $%d", i_rd, i_rt, i_rs);
	printf(", ( $%d = $%d << $%d )\n", i_rd, i_rt, i_rs);
}

static void
show_info_sra()
{
	printf("sra $%d, $%d, %d", i_rd, i_rt, i_sa);
	printf(", ( $%d = $%d >> %d )\n", i_rd, i_rt, i_sa);
}

static void
show_info_srav()
{
	printf("srav $%d, $%d, $%d", i_rd, i_rt, i_rs);
	printf(", ( $%d = $%d >> $%d )\n", i_rd, i_rt, i_rs);
}

static void
show_info_srl()
{
	printf("srl $%d, $%d, %d", i_rd, i_rt, i_sa);
	printf(", ( $%d = $%d >> %d )\n", i_rd, i_rt, i_sa);
}

static void
show_info_srlv()
{
	printf("srlv $%d, $%d, $%d", i_rd, i_rt, i_rs);
	printf(", ( $%d = $%d >> $%d )\n", i_rd, i_rt, i_rs);
}

static void
show_info_sub()
{
	printf("sub $%d, $%d, $%d", i_rd, i_rs, i_rt);
	printf(", ( $%d = $%d - $%d )\n", i_rd, i_rs, i_rt);
}

static void
show_info_subu()
{
	printf("subu $%d, $%d, $%d", i_rd, i_rs, i_rt);
	printf(", ( $%d = $%d - $%d )\n", i_rd, i_rs, i_rt);
}

static void
show_info_xor()
{
	printf("xor $%d, $%d, $%d", i_rd, i_rs, i_rt);
	printf(", ( $%d = $%d ^ $%d )\n", i_rd, i_rs, i_rt);
}

static void
show_info_xori()
{
	printf("xori $%d, $%d, %d", i_rt, i_rs, (short) i_imm);
	printf(", ( $%d = $%d ^ 0x%x )\n", i_rd, i_rs, i_imm);
}

static void
show_info_lui()
{
	printf("lui $%d, 0x%04x", i_rt, i_imm);
	printf(", ( $%d[32:16] = 0x%04x )\n", i_rt, i_imm);
}

static void
show_info_slt()
{
	printf("slt $%d, $%d, $%d", i_rd, i_rs, i_rt);
	printf(", ( $%d = $%d < $%d ? 1 : 0 )\n", i_rd, i_rs, (short) i_rt);
}

static void
show_info_slti()
{
	printf("slti $%d, $%d, %d", i_rt, i_rs, (short) i_imm);
	printf(", ( $%d = $%d < %d ? 1 : 0 )\n", i_rd, i_rs, (short) i_imm);
}

static void
show_info_sltu()
{
	printf("sltu $%u, $%u, $%u", i_rd, i_rs, i_rt);
	printf(", ( $%u = $%u < $%u ? 1 : 0 )\n", i_rd, i_rs, i_rt);
}

static void
show_info_sltiu()
{
	printf("sltiu $%d, $%d, %u", i_rt, i_rs, (int)(short)i_imm);
	printf(", ( $%d = $%d < %u ? 1 : 0 )\n", i_rd, i_rs, (int)(short)i_imm);
}

static void
show_info_beq()
{
	printf("beq $%d, $%d, %d", i_rs, i_rt, (short) i_imm);
	printf(", ( IF $%d == $%d, branch 0x%05x )\n", i_rs, i_rt,
		   (int) (short) i_imm << 2);
}

static void
show_info_bgez()
{
	printf("bgez $%d, %d", i_rs, (short) i_imm);
	printf(", ( IF $%d >= 0, branch 0x%05x )\n", i_rs,
		   (int) (short) i_imm << 2);
}

static void
show_info_bgezal()
{
	printf("bgezal $%d, %d", i_rs, (short) i_imm);
	printf(", ( IF $%d >= 0, branch 0x%05x and link )\n", i_rs,
		   (int) (short) i_imm << 2);
}

static void
show_info_bgtz()
{
	printf("bgtz $%d, %d", i_rs, (short) i_imm);
	printf(", ( IF $%d > 0, branch 0x%05x )\n", i_rs, (int) (short) i_imm << 2);
}

static void
show_info_blez()
{
	printf("blez $%d, %d", i_rs, (short) i_imm);
	printf(", ( IF $%d <= 0, branch 0x%05x )\n", i_rs,
		   (int) (short) i_imm << 2);
}

static void
show_info_bltzal()
{
	printf("bltzal $%d, %d", i_rs, (short) i_imm);
	printf(", ( IF $%d < 0, branch 0x%05x and link )\n", i_rs,
		   (int) (short) i_imm << 2);
}

static void
show_info_bltz()
{
	printf("bltz $%d, %d", i_rs, (short) i_imm);
	printf(", ( IF $%d < 0, branch 0x%05x )\n", i_rs, (int) (short) i_imm << 2);
}

static void
show_info_bne()
{
	printf("bne $%d, $%d, %d", i_rs, i_rt, (short) i_imm);
	printf(", ( IF $%d != $%d, branch 0x%05x )\n", i_rs, i_rt,
		   (int) (short) i_imm << 2);
}

static void
show_info_j()
{
	printf("j 0x?%07x\n", i_jaddr << 2);
}

static void
show_info_jal()
{
	printf("jal 0x?%07x\n", i_jaddr << 2);
}

static void
show_info_jalr()
{
	printf("jalr $%d, $%d\n", i_rd, i_rs);
}

static void
show_info_jr()
{
	printf("jr $%d\n", i_rs);
}

static void
show_info_lb()
{
	printf("lb $%d, %d($%d)\n", i_rt, (short) i_imm, i_rs);
}

static void
show_info_lbu()
{
	printf("lbu $%d, %d($%d)\n", i_rt, (short) i_imm, i_rs);
}

static void
show_info_lh()
{
	printf("lh $%d, %d($%d)\n", i_rt, (short) i_imm, i_rs);
}

static void
show_info_lhu()
{
	printf("lhu $%d, %d($%d)\n", i_rt, (short) i_imm, i_rs);
}

static void
show_info_lw()
{
	printf("lw $%d, %d($%d)\n", i_rt, (short) i_imm, i_rs);
}

static void
show_info_sb()
{
	printf("sb $%d, %d($%d)\n", i_rt, (short) i_imm, i_rs);
}

static void
show_info_sh()
{
	printf("sh $%d, %d($%d)\n", i_rt, (short) i_imm, i_rs);
}

static void
show_info_sw()
{
	printf("sw $%d, %d($%d)\n", i_rt, (short) i_imm, i_rs);
}

static void
show_info_break()
{
	printf("break\n");
}

static void
show_info_syscall()
{

	printf("syscall\n");
}

static void
show_info_mfhi()
{
	printf("mfhi $%d\n", i_rd);
}

static void
show_info_mflo()
{
	printf("mflo $%d\n", i_rd);
}

static void
show_info_mthi()
{
	printf("mthi $%d\n", i_rs);
}

static void
show_info_mtlo()
{
	printf("mtlo $%d\n", i_rs);
}

static void
show_info_mfc0()
{
	printf("mfc0 $%d, $%d", i_rt, i_rd);
	printf(", ( $%d = CPR$%d )\n", i_rt, i_rd);
}

static void
show_info_mtc0()
{
	printf("mtc0 $%d, $%d", i_rt, i_rd);
	printf(", ( CPR$%d = $%d )\n", i_rd, i_rt);
}

static void
show_info_eretx()
{
	printf("eretx\n");
}
