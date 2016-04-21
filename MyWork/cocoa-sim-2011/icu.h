struct RICU {
	unsigned int irr; /* ircode : irmmap : irmap */
	unsigned int imr;
	unsigned int isr;
};

#define ICU_OFFSET_IRR          0
#define ICU_OFFSET_IMR          4
#define ICU_OFFSET_ISR          8


#define ICU_IRR_MASK           0x000FFFFF
#define ICU_IRR_IRCODE_MASK    0x000F0000
#define ICU_IRR_IRCODE_SHIFT   16

#define ICU_IRR_IRMMAP_MASK    0x0000FF00
#define ICU_IRR_IRMMAP_SHIFT   8

#define ICU_IRR_IRMAP_MASK     0x000000FF
#define ICU_IRR_IRMAP_SHIFT    0


#define ICU_IMR_IM_MASK        0x000000FF
#define ICU_IMR_IM_SHIFT       0


#define ICU_ISR_MASK           0x070000FF
#define ICU_ISR_MASK_WRITE     0x07000000

#define ICU_ISR_ISO_MASK       0x07000000
#define ICU_ISR_ISO_SHIFT      24

#define ICU_ISR_ISR_MASK       0x000000FF
#define ICU_ISR_ISR_SHIFT      0


#define ICU_IRQ_TIMER0   7
#define ICU_IRQ_TIMER1   6
#define ICU_IRQ_TIMER2   5
#define ICU_IRQ_UART     4
#define ICU_IRQ_IOLET    3
//#define ICU_TO_CP0_IRQ   4



void icu_read_32(unsigned int address, unsigned int * data);
void icu_read_16(unsigned int address, unsigned short * data);
void icu_read_8(unsigned int address, unsigned char * data);
void icu_write_32(unsigned int address, unsigned int data);
void icu_write_16(unsigned int address, unsigned short data);
void icu_write_8(unsigned int address, unsigned char data);
void icu_reset(void);
void signal_interrupt(unsigned int intcode);
unsigned int check_icu_interrupt(void);
void show_icu_regs(void);


