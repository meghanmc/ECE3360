/*
 * Lab5.c
 *
 * Created: 4/4/2019 9:07:46 AM
 * Authors : McLaughlin, Meghan & McNamara, Dylan
 */ 
#define F_CPU 8000000L


// define baud rate as 9600
#define BAUD 9600

// define ADC Reference Voltage - 5.0 V
#define V_REF 5.0

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
const char sdata[] = "This string is in SRAM\n\r";
const char fdata[] PROGMEM = "This string is in FLASH\n\r";

// function prototypes
void usart_init(void);
void usart_prints(const char *ptr);
void usart_printf(const char *ptr);
void usart_putchar(const char c);
unsigned char usart_getchar(void);
void adc_init();
char echo(void);
uint16_t readADC();
void modeS();
void modeR();
float ADC_to_V(uint16_t adc_val);
unsigned char EEPROM_read(unsigned int uiAddress);
void EEPROM_write(unsigned int uiAddress, unsigned char ucData);
uint8_t getLow(uint16_t adc_val);
uint8_t getHigh(uint16_t adc_val);
void getUserString(char *str);


int main(void){	
	// initialize var
	float v;
	char str[25];
	
	// enable global interrupts
	sei();
	usart_init();
	adc_init();
	// TODO - remove these later
	//usart_prints(sdata);
	//usart_printf(fdata);
	while (1){
		usart_prints("\n\rSELECT A MODE: ");

		unsigned char user_sel = echo();
		if (user_sel == 'M' || user_sel == 'm' ){
			v = ADC_to_V(readADC());
			sprintf(str, "\n\rv = %.3f V", v);
			usart_prints(str);
		} else if (user_sel == 'S' || user_sel == 's') {
			usart_putchar(':');
			modeS();
		} else if (user_sel == 'R' || user_sel == 'r') {
			usart_putchar(':');
			modeR();
		} else if (user_sel == 'E' || user_sel == 'e') {
			usart_putchar(':');
			usart_prints("\n\rUser selected E");
		} else {
			usart_prints("\n\rUnrecognized Input");
		}
	}

	return(1);
}

// setup ADC
void adc_init(void){
	// set ADMUX0 as ADC input, set mode as AVcc with ext. capacitor at AREF pin
	ADMUX = (0<<REFS1) | (1<<REFS0);

	// ADEN = ADC Enable
	// ADSC = ADC Start Conversion
	// ADPS[2:0] = Prescalar Select - 64
	ADCSRA = (1<<ADEN)| (1<<ADPS2) | (1<<ADPS1);
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
	UCSR0C = (1<<UPM01) | (1<<USBS0);
	// SET THIS ON 2 SEPARATE LINES FOR SOME REASON?
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

char echo(void) {

	unsigned char c = usart_getchar();    // Get character
	usart_putchar(c);					// Echo it back
	return c; // return this 
}

// read ADC voltage level
// TODO convert to uint16_t
uint16_t readADC(){
	ADCSRA |= (1<< ADSC); // start conversion
	while (ADCSRA & (1<<ADSC)){ // wait for conversion to finish
	}
	uint8_t adc_low = ADCL;
	uint16_t adc_val = ADCH<<8 | adc_low;
	return adc_val;
}

void modeS(){
// Mode S - user input of the format "S:a,n,t" where:
//		a = eeprom start address (0 <= a <= 510)
//		n = number of measurements (1 <= n <= 20) 
//		t = time between measurements (1 <= t <= 10 s)
	float adc_val;
	int a; //todo remove these later
	int n;
	int t;
	
	char str[25];

	getUserString(str); // get a, n, t from the user
	int i = 0; // counter for parsing string
	int a_count = 0; // counter for the number of digits in a
	int n_count =0; // counter for the number of digits in n
	int t_count =0; // counter for the number of digits in r

	// Parse the string, splitting on commas
	while(str[i]!=','){
		// this value is a
		a_count++;
		i++;
	}
	i++;
	char a_str[a_count+1];//remember null terminator
	strncpy(a_str, str, a_count);
	a = atoi(a_str);
	while(str[i]!=','){
		// this value is n
		n_count++;
		i++;
	}
	char n_str[n_count+1];//remember null terminator
	strncpy(n_str, str+a_count+1,n_count);
	n = atoi(n_str);
	while(str[i]!='\0'){
		// this value is t
		t_count++;
		i++;
	}
	char t_str[t_count+1];//remember null terminator
	strncpy(t_str, str+a_count+1+n_count+1,t_count);
	t = atoi(t_str);

	// addresses should only be even numbers as each entry takes 2 address slots
	if (a%2) {
		a = a-1;
	}
		
	unsigned int address = (unsigned int) a;

	for (int i = 0; i < n; i++){
		// get ADC value
		adc_val = readADC();

		// Write to Address 
		EEPROM_write(address, (unsigned int) getHigh(adc_val));
		address++;
		EEPROM_write(address, (unsigned int) getLow(adc_val));
		address++;

		// Send user info about reading
		sprintf(str, "\n\rt = %d, v = %.3f V, addr: %d", i, ADC_to_V(adc_val), address-2); // address-2 because address has been incremented
		usart_prints(str);

		// wait for next measurement
		for (int x=0; x <t; x++){
			// wait one second
			_delay_ms(1000);
		}
	}
}

void modeR(){
// Mode R - user input of the format "R:a,n" where:
//		a = eeprom start address (0 <= a <= 510)
//		n = number of measurements (1 <= n <= 20)
	int a; //todo remove these later
	int n;
	char str[25];
	
	getUserString(str); // get a, n from the user
	int i = 0; // counter for parsing string
	int a_count = 0; // counter for the number of digits in a
	int n_count =0; // counter for the number of digits in n

		// Parse the string, splitting on commas
		while(str[i]!=','){
			// this value is a
			a_count++;
			i++;
		}
		i++;
		char a_str[a_count+1];//remember null terminator
		strncpy(a_str, str, a_count);
		a = atoi(a_str);
		while(str[i]!='\0'){
			// this value is n
			n_count++;
			i++;
		}
		char n_str[n_count+1];//remember null terminator
		strncpy(n_str, str+a_count+1,n_count);
		n = atoi(a_str);
	
	// addresses should only be even numbers as each entry takes 2 address slots
	if (a%2) {
		a = a-1;
	}
	
	int address = a;

	for (int i = 0; i < n; i++) {
		// Get stored value from EEPROM
		uint8_t adc_high = (uint8_t) EEPROM_read(address); // returns the high byte
		address++;
		uint8_t adc_low = (uint8_t) EEPROM_read(address); // returns the low byte
		address++;
		uint16_t adc = adc_high<<8 | adc_low;

		// Send user info about reading
		double v = ADC_to_V(adc);
		sprintf(str, "\n\rv = %.3f V", v);
		usart_prints(str);
	}

}

// From ATmega88PA Datasheet:
void EEPROM_write(unsigned int uiAddress, unsigned char ucData) {
/* Wait for completion of previous write */
while(EECR & (1<<EEPE))
;
/* Set up address and Data Registers */
EEAR = uiAddress;
EEDR = ucData;
/* Write logical one to EEMPE */
EECR |= (1<<EEMPE);
/* Start eeprom write by setting EEPE */
EECR |= (1<<EEPE);
}

// From ATmega88PA Datasheet:
unsigned char EEPROM_read(unsigned int uiAddress) {
/* Wait for completion of previous write */
while(EECR & (1<<EEPE))
;
/* Set up address register */
EEAR = uiAddress;
/* Start eeprom read by writing EERE */
EECR |= (1<<EERE);
/* Return data from Data Register */
return EEDR;
}

// Converts ADC register value to a float
float ADC_to_V(uint16_t adc_val){
	double adc_float = (double) adc_val;
	double v_val = (adc_float * 5.0) / 1024.0;
	return v_val;
}

uint8_t getLow(uint16_t adc_val){
	uint8_t low = adc_val & 0x00FF;
	return low;
}

uint8_t getHigh(uint16_t adc_val){
	uint8_t high = (adc_val>>8);
	return high;
}

void getUserString(char *str){
	// clear the string
	for (int i = 0; i<25;i++){
		str[i] = '\0';
	}
	char c;
	int i = 0;
	while(1){
		c = echo(); // read the value back to the user
		if (c == ' '){
			// endline, break loop
			str[i]='\0';
			return;
		} else {
			str[i] = c;
			i++;
		}
	}
}