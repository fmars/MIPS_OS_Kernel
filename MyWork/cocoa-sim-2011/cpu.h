/*
 * cpu.h
 * by wuxb
 *
 */

#define INST_CHECK(inst,mask,check) ((inst & mask) == check)

/*
 * entry point of each ex/int
 */
#define PC_ADDRESS_RESET               (0xBFC00000u)
#define PC_ADDRESS_INTERRPUT_BEV0      (0x80000200u)
#define PC_ADDRESS_INTERRPUT_BEV1      (0xBFC00400u)
#define PC_ADDRESS_EXCEPTION_BEV0      (0x80000180u)
#define PC_ADDRESS_EXCEPTION_BEV1      (0xBFC00380u)

/*
 * GPR's index
 */
#define GPR_ZERO   (0)
#define GPR_AT     (1)
#define GPR_V0     (2)
#define GPR_V1     (3)
#define GPR_A0     (4)
#define GPR_A1     (5)
#define GPR_A2     (6)
#define GPR_A3     (7)
#define GPR_T0     (8)
#define GPR_T1     (9)
#define GPR_T2     (10)
#define GPR_T3     (11)
#define GPR_T4     (12)
#define GPR_T5     (13)
#define GPR_T6     (14)
#define GPR_T7     (15)
#define GPR_S0     (16)
#define GPR_S1     (17)
#define GPR_S2     (18)
#define GPR_S3     (19)
#define GPR_S4     (20)
#define GPR_S5     (21)
#define GPR_S6     (22)
#define GPR_S7     (23)
#define GPR_T8     (24)
#define GPR_T9     (25)
#define GPR_K0     (26)
#define GPR_K1     (27)
#define GPR_GP     (28)
#define GPR_SP     (29)
#define GPR_FP     (30)
#define GPR_RA     (31)

/*
 * mask to get each part of instruction
 */
#define INST_MASK_31_26      (0xFC000000u)
#define INST_MASK_25_21      (0x03E00000u)
#define INST_MASK_20_16      (0x001F0000u)
#define INST_MASK_15_11      (0x0000F800u)
#define INST_MASK_10_6       (0x000007C0u)
#define INST_MASK_5_0        (0x0000003Fu)
#define INST_MASK_IMM        (0x0000FFFFu)
#define INST_MASK_JADDR      (0x03FFFFFFu)

#define INST_SPLIT_31_26(inst) ((inst&INST_MASK_31_26)>>26)
#define INST_SPLIT_25_21(inst) ((inst&INST_MASK_25_21)>>21)
#define INST_SPLIT_20_16(inst) ((inst&INST_MASK_20_16)>>16)
#define INST_SPLIT_15_11(inst) ((inst&INST_MASK_15_11)>>11)
#define INST_SPLIT_10_6(inst)  ((inst&INST_MASK_10_6)>>6)
#define INST_SPLIT_5_0(inst)   (inst&INST_MASK_5_0)
#define INST_SPLIT_IMM(inst)   (inst&INST_MASK_IMM)
#define INST_SPLIT_JADDR(inst) (inst&INST_MASK_JADDR)

#define INST_JOIN(f1,f2,f3,f4,f5,f6) \
(((f1<<26)&INST_MASK_31_26)|\
((f2<<21)&INST_MASK_25_21)|\
((f3<<16)&INST_MASK_20_16)|\
((f4<<11)&INST_MASK_15_11)|\
((f5<<6)&INST_MASK_10_6)|(f6&INST_MASK_5_0))

#define INST_JOIN_IMM(f1,f2,f3,imm) \
(((f1<<26)&INST_MASK_31_26)|\
((f2<<21)&INST_MASK_25_21)|\
((f3<<16)&INST_MASK_20_16)|\
(imm&INST_MASK_IMM))

#define INST_MASK_31_26_x00  (0x00000000u)
#define INST_MASK_31_26_x01  (0x04000000u)
#define INST_MASK_31_26_x10  (0x40000000u)

#define INST_MASK_5_0_ADD        (0x00000020u)
#define INST_MASK_5_0_ADDU       (0x00000021u)
#define INST_MASK_31_26_ADDI     (0x20000000u)
#define INST_MASK_31_26_ADDIU    (0x24000000u)
#define INST_MASK_5_0_AND        (0x00000024u)
#define INST_MASK_31_26_ANDI     (0x30000000u)
#define INST_MASK_5_0_DIV        (0x0000001Au)
#define INST_MASK_5_0_DIVU       (0x0000001Bu)
#define INST_MASK_5_0_MULT       (0x00000018u)
#define INST_MASK_5_0_MULTU      (0x00000019u)
#define INST_MASK_5_0_NOR        (0x00000027u)
#define INST_MASK_5_0_OR         (0x00000025u)
#define INST_MASK_31_26_ORI      (0x34000000u)
#define INST_MASK_5_0_SLL        (0x00000000u)
#define INST_MASK_5_0_SLLV       (0x00000004u)
#define INST_MASK_5_0_SRA        (0x00000003u)
#define INST_MASK_5_0_SRAV       (0x00000007u)
#define INST_MASK_5_0_SRL        (0x00000002u)
#define INST_MASK_5_0_SRLV       (0x00000006u)
#define INST_MASK_5_0_SUB        (0x00000022u)
#define INST_MASK_5_0_SUBU       (0x00000023u)
#define INST_MASK_5_0_XOR        (0x00000026u)
#define INST_MASK_31_26_XORI     (0x38000000u)
#define INST_MASK_31_26_LUI      (0x3C000000u)
#define INST_MASK_5_0_SLT        (0x0000002Au)
#define INST_MASK_5_0_SLTU       (0x0000002Bu)
#define INST_MASK_31_26_SLTI     (0x28000000u)
#define INST_MASK_31_26_SLTIU    (0x2C000000u)
#define INST_MASK_31_26_BEQ      (0x10000000u)
#define INST_MASK_20_16_BGEZ     (0x00010000u)
#define INST_MASK_20_16_BGEZAL   (0x00110000u)
#define INST_MASK_31_26_BGTZ     (0x1C000000u)
#define INST_MASK_31_26_BLEZ     (0x18000000u)
#define INST_MASK_20_16_BLTZAL   (0x00100000u)
#define INST_MASK_20_16_BLTZ     (0x00000000u)
#define INST_MASK_31_26_BNE      (0x14000000u)
#define INST_MASK_31_26_J        (0x08000000u)
#define INST_MASK_31_26_JAL      (0x0C000000u)
#define INST_MASK_5_0_JALR       (0x00000009u)
#define INST_MASK_5_0_JR         (0x00000008u)
#define INST_MASK_31_26_LB       (0x80000000u)
#define INST_MASK_31_26_LBU      (0x90000000u)
#define INST_MASK_31_26_LH       (0x84000000u)
#define INST_MASK_31_26_LHU      (0x94000000u)
#define INST_MASK_31_26_LW       (0x8C000000u)
#define INST_MASK_31_26_SB       (0xA0000000u)
#define INST_MASK_31_26_SH       (0xA4000000u)
#define INST_MASK_31_26_SW       (0xAC000000u)
#define INST_MASK_5_0_MFHI       (0x00000010u)
#define INST_MASK_5_0_MFLO       (0x00000012u)
#define INST_MASK_5_0_MTHI       (0x00000011u)
#define INST_MASK_5_0_MTLO       (0x00000013u)
#define INST_MASK_25_21_MFC0     (0x00000000u)
#define INST_MASK_25_21_MTC0     (0x00800000u)
#define INST_MASK_5_0_SYSCALL    (0x0000000Cu)
#define INST_MASK_5_0_BREAK      (0x0000000Du)
#define INST_MASK_25_21_ERETX    (0x02000000u)

#define INST_MASK_ADD      (0xFC0007FFu)
#define INST_MASK_ADDU     (0xFC0007FFu)
#define INST_MASK_ADDI     (0xFC000000u)
#define INST_MASK_ADDIU    (0xFC000000u)
#define INST_MASK_AND      (0xFC0007FFu)
#define INST_MASK_ANDI     (0xFC000000u)
#define INST_MASK_DIV      (0xFC00FFFFu)
#define INST_MASK_DIVU     (0xFC00FFFFu)
#define INST_MASK_MULT     (0xFC00FFFFu)
#define INST_MASK_MULTU    (0xFC00FFFFu)
#define INST_MASK_NOR      (0xFC0007FFu)
#define INST_MASK_OR       (0xFC0007FFu)
#define INST_MASK_ORI      (0xFC000000u)
#define INST_MASK_SLL      (0xFC00003Fu)
#define INST_MASK_SLLV     (0xFC0007FFu)
#define INST_MASK_SRA      (0xFC00003Fu)
#define INST_MASK_SRAV     (0xFC0007FFu)
#define INST_MASK_SRL      (0xFC00003Fu)
#define INST_MASK_SRLV     (0xFC0007FFu)
#define INST_MASK_SUB      (0xFC0007FFu)
#define INST_MASK_SUBU     (0xFC0007FFu)
#define INST_MASK_XOR      (0xFC0007FFu)
#define INST_MASK_XORI     (0xFC000000u)
#define INST_MASK_LUI      (0xFFE00000u)
#define INST_MASK_SLT      (0xFC0007FFu)
#define INST_MASK_SLTU     (0xFC0007FFu)
#define INST_MASK_SLTI     (0xFC000000u)
#define INST_MASK_SLTIU    (0xFC000000u)
#define INST_MASK_BEQ      (0xFC000000u)
#define INST_MASK_BGEZ     (0xFC1F0000u)
#define INST_MASK_BGEZAL   (0xFC1F0000u)
#define INST_MASK_BGTZ     (0xFC1F0000u)
#define INST_MASK_BLEZ     (0xFC1F0000u)
#define INST_MASK_BLTZAL   (0xFC1F0000u)
#define INST_MASK_BLTZ     (0xFC1F0000u)
#define INST_MASK_BNE      (0xFC000000u)
#define INST_MASK_J        (0xFC000000u)
#define INST_MASK_JAL      (0xFC000000u)
#define INST_MASK_JALR     (0xFC1F07FFu)
#define INST_MASK_JR       (0xFC1FFFFFu)
#define INST_MASK_LB       (0xFC000000u)
#define INST_MASK_LBU      (0xFC000000u)
#define INST_MASK_LH       (0xFC000000u)
#define INST_MASK_LHU      (0xFC000000u)
#define INST_MASK_LW       (0xFC000000u)
#define INST_MASK_SB       (0xFC000000u)
#define INST_MASK_SH       (0xFC000000u)
#define INST_MASK_SW       (0xFC000000u)
#define INST_MASK_MFHI     (0xFFFF07FFu)
#define INST_MASK_MFLO     (0xFFFF07FFu)
#define INST_MASK_MTHI     (0xFC1FFFFFu)
#define INST_MASK_MTLO     (0xFC1FFFFFu)
#define INST_MASK_MFC0     (0xFFE007FFu)
#define INST_MASK_MTC0     (0xFFE007FFu)
#define INST_MASK_SYSCALL  (0xFFFFFFFFu)
#define INST_MASK_BREAK    (0xFC00003Fu)
#define INST_MASK_ERETX    (0xFFFFFFFFu)


#define INST_CHECK_ADD      (0x00000020u)
#define INST_CHECK_ADDU     (0x00000021u)
#define INST_CHECK_ADDI     (0x20000000u)
#define INST_CHECK_ADDIU    (0x24000000u)
#define INST_CHECK_AND      (0x00000024u)
#define INST_CHECK_ANDI     (0x30000000u)
#define INST_CHECK_DIV      (0x0000001Au)
#define INST_CHECK_DIVU     (0x0000001Bu)
#define INST_CHECK_MULT     (0x00000018u)
#define INST_CHECK_MULTU    (0x00000019u)
#define INST_CHECK_NOR      (0x00000027u)
#define INST_CHECK_OR       (0x00000025u)
#define INST_CHECK_ORI      (0x34000000u)
#define INST_CHECK_SLL      (0x00000000u)
#define INST_CHECK_SLLV     (0x00000004u)
#define INST_CHECK_SRA      (0x00000003u)
#define INST_CHECK_SRAV     (0x00000007u)
#define INST_CHECK_SRL      (0x00000002u)
#define INST_CHECK_SRLV     (0x00000006u)
#define INST_CHECK_SUB      (0x00000022u)
#define INST_CHECK_SUBU     (0x00000023u)
#define INST_CHECK_XOR      (0x00000026u)
#define INST_CHECK_XORI     (0x38000000u)
#define INST_CHECK_LUI      (0x3C000000u)
#define INST_CHECK_SLT      (0x0000002Au)
#define INST_CHECK_SLTU     (0x0000002Bu)
#define INST_CHECK_SLTI     (0x28000000u)
#define INST_CHECK_SLTIU    (0x2C000000u)
#define INST_CHECK_BEQ      (0x10000000u)
#define INST_CHECK_BGEZ     (0x04010000u)
#define INST_CHECK_BGEZAL   (0x04110000u)
#define INST_CHECK_BGTZ     (0x1C000000u)
#define INST_CHECK_BLEZ     (0x18000000u)
#define INST_CHECK_BLTZAL   (0x04100000u)
#define INST_CHECK_BLTZ     (0x04000000u)
#define INST_CHECK_BNE      (0x14000000u)
#define INST_CHECK_J        (0x08000000u)
#define INST_CHECK_JAL      (0x0C000000u)
#define INST_CHECK_JALR     (0x00000009u)
#define INST_CHECK_JR       (0x00000008u)
#define INST_CHECK_LB       (0x80000000u)
#define INST_CHECK_LBU      (0x90000000u)
#define INST_CHECK_LH       (0x84000000u)
#define INST_CHECK_LHU      (0x94000000u)
#define INST_CHECK_LW       (0x8C000000u)
#define INST_CHECK_SB       (0xA0000000u)
#define INST_CHECK_SH       (0xA4000000u)
#define INST_CHECK_SW       (0xAC000000u)
#define INST_CHECK_MFHI     (0x00000010u)
#define INST_CHECK_MFLO     (0x00000012u)
#define INST_CHECK_MTHI     (0x00000011u)
#define INST_CHECK_MTLO     (0x00000013u)
#define INST_CHECK_MFC0     (0x40000000u)
#define INST_CHECK_MTC0     (0x40800000u)
#define INST_CHECK_SYSCALL  (0x0000000Cu)
#define INST_CHECK_BREAK    (0x0000000Du)
#define INST_CHECK_ERETX    (0x42000018u)

#define DECODE_TYPE_NULL    0
#define DECODE_TYPE_OP      1
#define DECODE_TYPE_NEXT    2

/* PC is so special that that can play with him... */
struct RCPU {
	unsigned int gpr[32];
	unsigned int hi;
	unsigned int lo;
};

struct DECODE {
	unsigned int type;
	unsigned int mask;
	unsigned int check;
	void(* op)(void);
	struct DECODE * next;
	unsigned int mask_next;
	unsigned int shift_next;
	char * name;
};

void cpu_reset(void);
void execute_cycle(void);
void show_cpu_regs(void);
