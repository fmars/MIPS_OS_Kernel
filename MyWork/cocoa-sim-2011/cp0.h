/*
 * cp0.h
 * by wuxb
 *
 */

/* Reset entry. */
#define ENTRY_INT_BEV_0     0x80000200
#define ENTRY_INT_BEV_1     0xBFC00400
#define ENTRY_EX_BEV_0      0x80000180
#define ENTRY_EX_BEV_1      0xBFC00380

#define CP0_REGID_BADVADDR  8
#define CP0_REGID_SR        12
#define CP0_REGID_CAUSE     13
#define CP0_REGID_EPC       14
#define CP0_REGID_PRID      15


#define CP0_SR_MASK         0x0040FF01

#define CP0_SR_BEV_MASK     0x00400000
#define CP0_SR_BEV_SHIFT    22

#define CP0_SR_IM_MASK      0x0000FF00
#define CP0_SR_IM_SHIFT     8

#define CP0_SR_IE_MASK      0x00000001
#define CP0_SR_IE_SHIFT     0


#define CP0_CAUSE_MASK      0x0000FF7C
#define CP0_CAUSE_MASK_W    0x00000300

#define CP0_CAUSE_IP_MASK   0x0000FF00
#define CP0_CAUSE_IP_SHIFT  8

#define CP0_CAUSE_EC_MASK   0x0000007C
#define CP0_CAUSE_EC_SHIFT  2

// exception code
#define CP0_CAUSE_EC_INT    0
#define CP0_CAUSE_EC_ADEL   4
#define CP0_CAUSE_EC_ADES   5
#define CP0_CAUSE_EC_SC     8
#define CP0_CAUSE_EC_BP     9
#define CP0_CAUSE_EC_RI     10
#define CP0_CAUSE_EC_ALU    12

#define CP0_PRID            0x12345678


struct RCP0 {
	unsigned int sr;
	unsigned int cause;
	unsigned int epc;
	unsigned int prid;
	unsigned int badvaddr;
};

void cp0_reset(void);
void read_cp0_reg(unsigned int id, unsigned int * value);
void write_cp0_reg(unsigned int id, unsigned int value);
void signal_exception(unsigned int exccode, unsigned int epc, unsigned int badvaddr);
void check_cp0_exception(void);
unsigned int exception_return(void);
void show_cp0_regs(void);
