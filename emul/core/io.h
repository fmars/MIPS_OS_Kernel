/*
 * !this file should not be included.
 * io.h
 * wuxb 2008-1-25
 *
 * ic---------0007
 * tc---------0049
 * uart-------0086
 * 
 * 
 */

/*
 * ICR-Interrupt Contral Register
 */
struct RIC {
	unsigned int irr;
	unsigned int imr;
	unsigned int isr;
};

/*
 * way def of IRR
 */
#define IC_IRR_IRC_MAX      19
#define IC_IRR_IRC_MIN      16
#define IC_IRR_IRMMAP_MAX   15
#define IC_IRR_IRMMAP_MIN   8
#define IC_IRR_IRMAP_MAX    7
#define IC_IRR_IRMAP_MIN    0
#define IC_IRR_MASK         0x000FFFFF

/*
 * way def of IMR
 */
#define IC_IMR_IM_MAX       7
#define IC_IMR_IM_MIN       0
#define IC_IMR_MASK         0x000000FF

/*
 * way def of ISR
 */
#define IC_ISR_ISO_MAX      26
#define IC_ISR_ISO_MIN      24
#define IC_ISR_ISR_MAX      7
#define IC_ISR_ISR_MIN      0
#define IC_ISR_MASK         0x070000FF

/*
 * Time Counter
 */
struct RTC {
	unsigned int tc0_ctrl;
	unsigned int tc0_preset;
	unsigned int tc0_count;
	unsigned int tc1_ctrl;
	unsigned int tc1_preset;
	unsigned int tc1_count;
	unsigned int tc2_ctrl;
	unsigned int tc2_preset;
	unsigned int tc2_count;
};

/*
 * way def of tc*_ctrl
 */
#define TC_CTRL_MODE_MAX     2
#define TC_CTRL_MODE_MIN     1
#define TC_CTRL_ENABLE       0
#define TC_CTRL_MASK         0x00000007

/*
 * way def of tc*_preset
 */
#define TC_PRESET_PRESET_MAX 31
#define TC_PRESET_PRESET_MIN 0
#define TC_PRESET_MASK       0xFFFFFFFF

/*
 * way def of tc*_count
 */
#define TC_COUNT_COUNT_MAX   31
#define TC_COUNT_COUNT_MIN   0
#define TC_COUNT_MASK        0xFFFFFFFF

/*
 * UART-Universal Asynchronous Receiver Transmitter
 */
struct RUART {
	unsigned int data;
	unsigned int ier;	/* int enable */
	unsigned int iir;	/* int identfy */
	unsigned int lcr;	/* line contral */
	unsigned int lsr;	/* line status */
	unsigned int div_snd;
};

/*
 * way def of data
 */
#define UART_DATA_DATA_MAX    7
#define UART_DATA_DATA_MIN    0
#define UART_DATA_MASK        0x000000FF

/*
 * way def of ier
 */
#define UART_IER_ELSI         2
#define UART_IER_ETBEI        1
#define UART_IER_ERBI         0
#define UART_IER_MASK         0x00000007

/*
 * way def of iir
 */
#define UART_IIR_INTID_MAX    2
#define UART_IIR_INTID_MIN    1
#define UART_IIR_IP           0
#define UART_IER_MASK         0x00000007

/*
 * way def of lcr
 */
#define UART_LCR_BC           6
#define UART_LCR_SP           5
#define UART_LCR_EPS          4
#define UART_LCR_PEN          3
#define UART_LCR_STB          2
#define UART_LCR_WLS_MAX      1
#define UART_LCR_WLS_MIN      0
#define UART_LCR_MASK         0x0000007F

/*
 * way def of lsr
 */
#define UART_LSR_THRE         5
#define UART_LSR_FE           3
#define UART_LSR_PE           2
#define UART_LSR_OE           1
#define UART_LSR_DR           0
#define UART_LSR_MASK         0x0000002F

/*
 * way def of divr
 */
#define UART_DIVR_DIVR_MAX    7
#define UART_DIVR_DIVR_MIN    0
#define UART_DIVR_MASK        0x000000FF

/*
 * way def of divs
 */
#define UART_DIVS_DIVS_MAX    15
#define UART_DIVS_DIVS_MIN    0
#define UART_DIVS_MASK        0x0000FFFF
