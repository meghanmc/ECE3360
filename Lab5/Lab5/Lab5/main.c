/*
 * Lab5.c
 *
 * Created: 4/4/2019 9:07:46 AM
 * Authors : McLaughlin, Meghan & McNamara, Dylan
 */ 

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

// define baud rate as 9600
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>



int main(void)
{
	// enable global interrupts
	sei();
	
	usart_init();
	//usart_prints(sdata)
	//usart_printf(fdata)
}

// initialize the USART
void usart_init()
{
	// set baud rate
	UBRR0H = (unsigned char) (MYUBRR>>8);
	UBRR0L = (unsigned char) MYUBRR;
	
	// enable Rx and Tx
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	
	
	
	
	// 8 data bits, odd parity, 2 stop bits 
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00) | (1<<UPM01) | (1<<UPM00) | (1<<USBS0);
	
	
}

// print a string from SRAM
void usart_prints(char sdata[])
{
	
}

// print a string from FLASH
void usart_printf(char fdata)
{
	// see data sheet pg 160 ish
	
}

// read ADC voltage level
int readADC()
{
	
}


// TODO - make some interrupt that acknowledges when there is a value in the URDn (receiving) register
// when sending data, place data in the UDR (output register). When all of the bits have been shifted, the UDRE (USART Data Register Empty) flag will be set - can use an interrupt for that!!
// URD0 is the USART receive register
// UDRE in UCSR0A - USART Data Register empty flag
// 


 
