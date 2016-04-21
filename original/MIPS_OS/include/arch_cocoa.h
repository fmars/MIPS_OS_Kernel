#ifndef __ARCH_COCOA_H
#define __ARCH_COCOA_H

//  clock frequency
#define MIPS_FREQ           25              // 25MHZ
#define CYCLE_INST          10
#define LOOP_1S             (1000000000/((1000/MIPS_FREQ)*CYCLE_INST*12))
#define LOOP_DELAY          (LOOP_1S/2)

/* MIPS processor core */
// CP0's status register
#define BIT_HW7             0x00008000              // hardware interrupt 7's bit position
#define BIT_HW6             0x00004000              // hardware interrupt 6's bit position
#define BIT_HW5             0x00002000              // hardware interrupt 5's bit position
#define BIT_HW4             0x00001000              // hardware interrupt 4's bit position
#define BIT_HW3             0x00000800              // hardware interrupt 3's bit position
#define BIT_HW2             0x00000400              // hardware interrupt 2's bit position
#define BIT_IE              0x00000001              // global interrupt enable's bit position

// CP0's cause register
#define CAUSE_EXCCODE(x)    (((x)&0xFC)>>2)     // get ExcCode field of cause register
#define EXC_INT             0                   // cause : interrupt
#define EXC_SC              8                   // cause : system call
#define EXC_BP              9                   // cause : break

#define IS_HWINT_7(cause, sr)  ((cause)&BIT_HW7) & ((sr)&BIT_HW7)       // hardware interrupt 7 is pending
#define IS_HWINT_6(cause, sr)  ((cause)&BIT_HW6) & ((sr)&BIT_HW6)       // hardware interrupt 6 is pending
#define IS_HWINT_5(cause, sr)  ((cause)&BIT_HW5) & ((sr)&BIT_HW5)       // hardware interrupt 5 is pending
#define IS_HWINT_4(cause, sr)  ((cause)&BIT_HW4) & ((sr)&BIT_HW4)       // hardware interrupt 4 is pending
#define IS_HWINT_3(cause, sr)  ((cause)&BIT_HW3) & ((sr)&BIT_HW3)       // hardware interrupt 3 is pending
#define IS_HWINT_2(cause, sr)  ((cause)&BIT_HW2) & ((sr)&BIT_HW2)       // hardware interrupt 2 is pending


/* Device address */
#define BASE_DEV            0xA0000000

#define MINOR_BASE_INTC     0x100
// TMR0
#define MINOR_BASE_TC0      0x200
#define MINOR_BASE_TC1      0x300
#define MINOR_BASE_TC2      0x400
    #define OFF_TC_CTRL     0x0
    #define OFF_TC_PRESET   0x4
    #define OFF_TC_COUNT    0x8
// UART
#define MINOR_BASE_UART0    0x500
    #define UART_DATA       0x0
    #define UART_IER        0x4
    #define UART_IIR        0x8
    #define UART_LCR        0xC
    #define UART_LSR        0x10
    #define UART_DIV_RCV    0x14
        #define BAUD_RCV_9600       (MIPS_FREQ*1000000/((9600+1)*8))
        #define BAUD_RCV_19200      (MIPS_FREQ*1000000/((19200+1)*8))
        #define BAUD_RCV_38400      (MIPS_FREQ*1000000/((38400+1)*8))
        #define BAUD_RCV_57600      (MIPS_FREQ*1000000/((57600+1)*8))
        #define BAUD_RCV_115200     (MIPS_FREQ*1000000/((115200+1)*8))
    #define UART_DIV_SND    0x18
        #define BAUD_SND_9600       (MIPS_FREQ*1000000/(9600+1))
        #define BAUD_SND_19200      (MIPS_FREQ*1000000/(19200+1))
        #define BAUD_SND_38400      (MIPS_FREQ*1000000/(38400+1))
        #define BAUD_SND_57600      (MIPS_FREQ*1000000/(57600+1))
        #define BAUD_SND_115200     (MIPS_FREQ*1000000/(115200+1))
// LED
#define MINOR_BASE_LED      0x700

typedef struct {
    unsigned int    data ;
    unsigned int    ier ;
    unsigned int    iir ;
    unsigned int    lcr ;
    struct {
/*
 * Note: since the first bit defined must be the LSB!!!!
 */
        unsigned dr     : 1 ;   // receive data ready
        unsigned oe     : 1 ;   // receive buffer overflow
        unsigned pe     : 1 ;   // parity error
        unsigned fe     : 1 ;   // data format error
        unsigned        : 1 ;   // reserved
        unsigned thre   : 1 ;   // send buffer empty
        unsigned        : 26 ;  // reserved
        } lsr ;
    unsigned int    div_rcv ;
    unsigned int    div_snd ;
} cocoa_uart ; 

#endif  /* __ARCH_COCOA_H */