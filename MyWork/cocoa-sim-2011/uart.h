/*
 * uart.h, by wuxb
 */


struct RUART {
	unsigned int data;
	unsigned int ier;	/* int enable.   */
	unsigned int iir;	/* int identfy.  */
	unsigned int lcr;	/* line contral. */
	unsigned int lsr;	/* line status.  */
	unsigned int divr;  /* div receive.  */
	unsigned int divs;  /* div send.     */
};

#define UART_OFFSET_DATA      0
#define UART_OFFSET_IER       4
#define UART_OFFSET_IIR       8
#define UART_OFFSET_LCR       12
#define UART_OFFSET_LSR       16
#define UART_OFFSET_DIVR      20
#define UART_OFFSET_DIVS      24

#define UART_DATA_MASK        0x000000FF

#define UART_IER_MASK         0x00000007
#define UART_IER_ELSI_MASK    0x00000004
#define UART_IER_ETBEI_MASK   0x00000002
#define UART_IER_ERBI_MASK    0x00000001

#define UART_IIR_MASK         0x00000007
#define UART_IIR_INTID_MASK   0x00000006
#define UART_IIR_IP_MASK      0x00000001

#define UART_LCR_MASK         0x0000007F
#define UART_LCR_BC_MASK      0x00000040
#define UART_LCR_SP_MASK      0x00000020
#define UART_LCR_EPS_MASK     0x00000010
#define UART_LCR_PEN_MASK     0x00000008
#define UART_LCR_STB_MASK     0x00000004
#define UART_LCR_WLS_MASK     0x00000003


#define UART_LSR_MASK         0x0000002F
#define UART_LSR_THRE_MASK    0x00000020
#define UART_LSR_FE_MASK      0x00000008
#define UART_LSR_PE_MASK      0x00000004
#define UART_LSR_OE_MASK      0x00000002
#define UART_LSR_DR_MASK      0x00000001


#define UART_DIVR_MASK        0x000000FF

#define UART_DIVS_MASK        0x0000FFFF


void uart_reset(void);
void uart_read_32(unsigned int address, unsigned int * data);
void uart_read_16(unsigned int address, unsigned short * data);
void uart_read_8(unsigned int address, unsigned char * data);
void uart_write_32(unsigned int address, unsigned int data);
void uart_write_16(unsigned int address, unsigned short data);
void uart_write_8(unsigned int address, unsigned char data);
void show_uart_regs(void);





