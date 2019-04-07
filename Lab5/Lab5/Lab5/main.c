/*
 * Lab5.c
 *
 * Created: 4/4/2019 9:07:46 AM
 * Authors : McLaughlin, Meghan & McNamara, Dylan
 */ 

#define F_CPU 8000000L


// define baud rate as 9600
#define BAUD 9600

// set up receive buffer
#define RX_BUFFER_SIZE 64 // TODO does this size matter?
unsigned char rx_buffer[RX_BUFFER_SIZE];
volatile unsigned char rx_buffer_head;
volatile unsigned char rx_buffer_tail; // TODO does this need volatile keyword??

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
unsigned char usart_getchar(void);
void echo4(void);
int readADC(void);


int main(void){	
	// enable global interrupts
	sei();
	adc_init();
	usart_init();
	usart_prints(sdata);
	usart_printf(fdata);
		while (1){
			usart_prints("\n SELECT A MODE: M, S, R, E\n");

			unsigned char user_sel = usart_getchar();
			if (user_sel == 'M'){
				usart_prints("User selected MEASURE\n");
				} else if (user_sel == 'S') {
				usart_prints("User selected STORE\n");
				} else if (user_sel == 'R') {
				usart_prints("User selected RETRIEVE\n");
				} else if (user_sel == 'E') {
				usart_prints("User selected E\n");
				} else {
				usart_prints("Unrecognized Input\n");
			}
	}

	return(1);
}

// setup ADC
void adc_init(void){
	ADMUX = (0<<REFS1) | (1<<REFS0) ;

	// ADEN = ADC Enable
	// ADSC = ADC Start Conversion
	// ADATE = ADATE ADC Auto Trigger Enable

	ADCSRA = (1<<ADEN) | (1<<ADATE)
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
	// TODO - per lab specifications, should be 8 bit, even, 2 stop bits
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

unsigned char usart_getchar(void){
	unsigned char ch;

	while (rx_buffer_tail == rx_buffer_head){
		; //while the buffer is empty, wait
	}
	ch = rx_buffer[rx_buffer_tail];
	if (rx_buffer_tail == RX_BUFFER_SIZE-1){
		rx_buffer_tail = 0;
	} else {
		rx_buffer_tail++;
	}
	return ch;

}

ISR(USART_RX_vect)
{
	// UART receive interrupt handler.
	char c = UDR0;
	rx_buffer[rx_buffer_head] = c;
	if (rx_buffer_head == RX_BUFFER_SIZE - 1)
	rx_buffer_head = 0;
	else
	rx_buffer_head++;
}

void echo4(void) {
   for (int i=0;i<=4-1;i++){
	   unsigned char c = usart_getchar();    // Get character
	   usart_putchar(c);					// Echo it back
   }
}

// read ADC voltage level
int readADC(){
	return 0;
	int adc = 
}