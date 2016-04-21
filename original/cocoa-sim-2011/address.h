/*
 * address.h
 * by wuxb
 */


/* 
 * kuseg: 2GB,[0x00000000] -- [0x7FFFFFFF], Undefined.
 */
#define KUSEG_OFFSET   (0x00000000u)
#define KUSEG_SIZE     (0x80000000u)

/*
 * kseg0: 512MB,[0x80000000] -- [0xBFFFFFFF], for memory.
 */
#define KSEG0_OFFSET   (0x80000000u)
#define KSEG0_SIZE     (0x20000000u)

/*
 * kseg1: 512MB,[0xA0000000] -- [0xBFFFFFFF], for flash and I/O device.
 */
#define KSEG1_OFFSET   (0xA0000000u)
#define KSEG1_SIZE     (0x20000000u)

/* 
 * kseg2: 1GB,[0xC0000000] -- [0xFFFFFFFF], Undefined.
 */
#define KSEG2_OFFSET   (0xC0000000u)
#define KSEG2_SIZE     (0x40000000u)

/* 
 * offset in kseg0
 */
#define MEMORY_OFFSET  (0x00000000u)

/* 
 * size of memory, mutable
 */
#define MEMORY_SIZE    (memory_size)
/*
 * offset in kseg1
 */
#define CONFREG_OFFSET (0x00000000u)
#define INTCTRL_OFFSET (0x00000100u)
#define TIMER_OFFSET   (0x00000200u)
#define TIMER1_OFFSET  (0x00000300u)
#define TIMER2_OFFSET  (0x00000400u)
#define UART_OFFSET    (0x00000500u)
#define RESV_OFFSET    (0x00000600u)
#define LED_OFFSET     (0x00000700u)
#define SWITCH_OFFSET  (0x00000800u)

#define FLASH_OFFSET   (0x1F000000u)

/*
 * size of each mapping of I/O device
 */
#define IO_SIZE        (0x00000100u)

/*
 * size of flash, 16MB
 */
//#define FLASH_SIZE     0x01000000
#define FLASH_SIZE     (flash_size)
/*
 * exact address map
 */

/* [0x80000000] -- [0x87FFFFFF] */
#define MEMORY_START   (KSEG0_OFFSET + MEMORY_OFFSET)
#define MEMORY_END     (KSEG0_OFFSET + MEMORY_OFFSET + MEMORY_SIZE -1u)

/* [0xA0000000] -- [0xA00000FF] */
#define CONFREG_START  (KSEG1_OFFSET + CONFREG_OFFSET)
#define CONFREG_END    (KSEG1_OFFSET + CONFREG_OFFSET + IO_SIZE -1u)

/* [0xA0000100] -- [0xA00001FF] */
#define INTCTRL_START  (KSEG1_OFFSET + INTCTRL_OFFSET)
#define INTCTRL_END    (KSEG1_OFFSET + INTCTRL_OFFSET + IO_SIZE -1u)

/* [0xA0000200] -- [0xA00002FF] */
#define TIMER_START    (KSEG1_OFFSET + TIMER_OFFSET)
#define TIMER_END      (KSEG1_OFFSET + TIMER_OFFSET + IO_SIZE -1u)

/* [0xA0000300] -- [0xA00003FF] */
#define TIMER1_START    (KSEG1_OFFSET + TIMER1_OFFSET)
#define TIMER1_END      (KSEG1_OFFSET + TIMER1_OFFSET + IO_SIZE -1u)

/* [0xA0000400] -- [0xA00004FF] */
#define TIMER2_START    (KSEG1_OFFSET + TIMER2_OFFSET)
#define TIMER2_END      (KSEG1_OFFSET + TIMER2_OFFSET + IO_SIZE -1u)

/* [0xA0000500] -- [0xA00005FF] */
#define UART_START     (KSEG1_OFFSET + UART_OFFSET)
#define UART_END       (KSEG1_OFFSET + UART_OFFSET + IO_SIZE -1u)

/* [0xA0000600] -- [0xA00006FF] */
#define RESV_START     (KSEG1_OFFSET + RESV_OFFSET)
#define RESV_END       (KSEG1_OFFSET + RESV_OFFSET + IO_SIZE -1u)

/* [0xA0000700] -- [0xA00007FF] */
#define LED_START      (KSEG1_OFFSET + LED_OFFSET)
#define LED_END        (KSEG1_OFFSET + LED_OFFSET + IO_SIZE -1u)

/* [0xA0000800] -- [0xA00008FF] */
#define SWITCH_START   (KSEG1_OFFSET + SWITCH_OFFSET)
#define SWITCH_END     (KSEG1_OFFSET + SWITCH_OFFSET + IO_SIZE -1u)

/* [0xBF000000] -- [0xBFFFFFFF] */
#define FLASH_START    (KSEG1_OFFSET + FLASH_OFFSET)
#define FLASH_END      (KSEG1_OFFSET + FLASH_OFFSET + FLASH_SIZE -1u)

/* return 0 means ok, else return the exccode of exception. */
unsigned int address_read_32(unsigned int addr, unsigned int * data);
unsigned int address_read_16(unsigned int addr, unsigned short * data);
unsigned int address_read_8(unsigned int addr, unsigned char * data);
unsigned int address_write_32(unsigned int addr, unsigned int data);
unsigned int address_write_16(unsigned int addr, unsigned short data);
unsigned int address_write_8(unsigned int addr, unsigned char data);

