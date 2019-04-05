/*
 * Lab5.c
 *
 * Created: 4/4/2019 9:07:46 AM
 * Authors : McLaughlin, Meghan & McNamara, Dylan
 */ 

#define F_CPU 8000000L


// define baud rate as 9600
#define BAUD 9600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <stdio.h>

// store strings in SRAM and FLASH
const char sdata[] = "This string is in SRAM\n";
const char fdata[] PROGMEM = "This string is in FLASH\n";

// function prototypes
void usart_init(void);
void usart_prints(const char *ptr);
void usart_printf(const char *ptr);
void usart_putchar(const char c);
int readADC(void);


int main(void){
	
	// enable global interrupts
	sei();
	
	usart_init();
	usart_prints(sdata);
	usart_printf(fdata);
	usart_prints("This string is defined in main()\n");

	while(1)
		;
	return(1);
}

// initialize the USART
void usart_init(void){
	// set baud rate
	unsigned short ubrr;
	ubrr = (double) (F_CPU / (BAUD*16.0) - 1.0);
	UBRR0H = (ubrr & 0xFF00);
	UBRR0L = (ubrr & 0x00FF);

	// enable Rx and Tx, Rx complete interrupt enabled
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);

	//Asynchronous USART, 8 data bits, NO parity, 1 stop bits
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

// print a string from SRAM
void usart_prints(const char *ptr){
	while (*ptr) {
		while(!(UCSR0A & (1<<UDRE0)))
			; //wait for data register to be ready
		cli();
		UDR0 = *(ptr++);
		sei();
	}
}

// print a string from FLASH
void usart_printf(const char *ptr){
	char c;
	//
	while(pgm_read_byte_near(ptr)) {
		c = pgm_read_byte_near(ptr++);
		usart_putchar(c);
	}
}

// writes a single character of a string
void usart_putchar(const char c){
	// UDRE0 = USART Data Register 0 empty flag
	// Wait until the data register is ready for data
	while(!(UCSR0A & (1<<UDRE0)))
		; //wait for data register to be ready
	cli();
	UDR0 = c;
	sei();
}

ISR(USART_RX_vect)
{
	// UART receive interrupt handler.
	// To do: check and warn if buffer overflows.
	/*
	char c = UDR0;
	rx_buffer[rx_buffer_head] = c;
	if (rx_buffer_head == RX_BUFFER_SIZE - 1)
	rx_buffer_head = 0;
	else
	rx_buffer_head++;
	*/
}

// read ADC voltage level
int readADC(){
	return 0;
}


// TODO - make some interrupt that acknowledges when there is a value in the URDn (receiving) register
// when sending data, place data in the UDR (output register). When all of the bits have been shifted, the UDRE (USART Data Register Empty) flag will be set - can use an interrupt for that!!
// URD0 is the USART receive register
// UDRE in UCSR0A - USART Data Register empty flag
// 


 
