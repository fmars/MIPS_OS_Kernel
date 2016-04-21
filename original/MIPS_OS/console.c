/*
 *  $Id: hello.c,v 1.3 2006/05/22 04:53:52 debug Exp $
 *
 *  GXemul demo:  Hello World
 *
 *  This file is in the Public Domain.
 */

#include "include/dev_cons.h"

/*  Note: The ugly cast to a signed int (32-bit) causes the address to be
	sign-extended correctly on MIPS when compiled in 64-bit mode  */
#define	PHYSADDR_OFFSET		((signed int)0xa0000000)


#define	PUTCHAR_ADDRESS		(PHYSADDR_OFFSET +		\
				DEV_CONS_ADDRESS + DEV_CONS_PUTGETCHAR)
#define	HALT_ADDRESS		(PHYSADDR_OFFSET +		\
				DEV_CONS_ADDRESS + DEV_CONS_HALT)



//
#define MIPS_FREQ           25              // 25MHZ
#define CYCLE_INST          10
#define LOOP_1S             (1000000000/((1000/MIPS_FREQ)*CYCLE_INST*12))
#define LOOP_DELAY          (LOOP_1S/2)

//
#define BASE_DEV            0xA0000000

// UART
#define MINOR_BASE_UART0    0x500
    #define UART_DATA       0x0
    #define UART_DIV_SND    0x18

// 
#define MINOR_BASE_LED      0x700


#include "include/arch_cocoa.h"

extern  cocoa_uart  *P_UART0 ; 


void    delay_loop( void ) ;


void printcharc(unsigned char ch)
{
	cocoa_uart  *p_uart ;

	p_uart  = (cocoa_uart *)(BASE_DEV + MINOR_BASE_UART0) ;

    while ( !p_uart->lsr.thre )
        ;
	p_uart->data = ch ;
	
	//*((volatile unsigned char *) PUTCHAR_ADDRESS) = ch;
	//delay_loop();
}


void halt(void)
{
	*((volatile unsigned char *) HALT_ADDRESS) = 0;
}



void printstr(unsigned char *s)
{
	while (*s)
	   printcharc(*s++);
}



void    delay_loop(void)
{
    int j ;
    for ( j=0; j<LOOP_DELAY/1000; j++ ) 
        ;
}





