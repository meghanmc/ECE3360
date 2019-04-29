/*
 * Project.c
 *
 * Created: 4/19/2019 12:24:42 PM
 * Author : mlmclaughlin & dtmcnamara
 */ 

#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "nlcd.h"		// library for displaying to Nokia 5110 LCD

#define COL0 3*6
#define COL1 7*6
#define COL2 11*6
#define ROW0 1*8
#define ROW1 3*8
#define ROW2 5*8

/*
LCD INFO: 
	48 px-rows
	84 px-col
	8 px/char (hight/row)
	6 px/character (width/col)
	6x14 characters
*/

// our functions
void board_int(LcdNokia *lcd_ptr);
void write_token(LcdNokia *lcd_ptr, bool is_x, uint8_t col, uint8_t row);
void write_player_turn(LcdNokia *lcd_ptr, bool x_turn);
void keypad_init();
uint8_t getKeyPressed();

int main(void)
{
// Program Initialization
	struct lcd_nokia lcd;
	// Board is initialized as -1's. When tokens are placed, 0 = O token, 1 is X token
	int board[3][3] = {
		{-1,-1,-1},
		{-1,-1,-1},
		{-1,-1,-1}
	};
	bool x_turn = true;
	_delay_ms(500); // give the LCD time to power up
	keypad_init();
	lcd_nokia_init();
	lcd_nokia_clear(&lcd);
	board_int(&lcd);
	
// End Initialization

// testing keypad
	uint8_t key;
	while(1){
		key = getKeyPressed();
		if (key < 0xFF){
			lcd_nokia_set_cursor(&lcd, 0, 0);
			lcd_nokia_write_string(&lcd, "Key Pressed", 1);
			lcd_nokia_render(&lcd);
		}

	}


//end testing keypad

// Begin Game
	
	// Example
	_delay_ms(1000);
	
	write_token(&lcd, 1, COL0, ROW2);
	x_turn = !x_turn;
	write_player_turn(&lcd, x_turn);
	_delay_ms(1000);
	
	write_token(&lcd, 0, COL1, ROW2);
	x_turn = !x_turn;
	write_player_turn(&lcd, x_turn);
	_delay_ms(1000);
	
	write_token(&lcd, 1, COL0, ROW0);
	x_turn = !x_turn;
	write_player_turn(&lcd, x_turn);
	_delay_ms(1000);
	
	write_token(&lcd, 0, COL2, ROW2);
	x_turn = !x_turn;
	write_player_turn(&lcd, x_turn);
	// End Example

	for (;;) {
		_delay_ms(1000);
	}

}

void board_int(LcdNokia *lcd_ptr){
	char * arr[] = {
		"  Tic-Tac-Toe  ",
		"     |   |     ",
		"  -----------  ",
		"     |   |     ",
		"  -----------  ",
		"     |   |     "
	};
	
	for (int i = 0; i < 6; i++){
		lcd_nokia_set_cursor(lcd_ptr, 0, (8*i));
		lcd_nokia_write_string(lcd_ptr, arr[i], 1);
	}
	
	lcd_nokia_render(lcd_ptr);
	return;
}

void write_token(LcdNokia *lcd_ptr, bool x_turn, uint8_t col, uint8_t row){
	lcd_nokia_set_cursor(lcd_ptr, col, row);
	if(x_turn){
		lcd_nokia_write_string(lcd_ptr, "X", 1);
	} else {
		lcd_nokia_write_string(lcd_ptr, "O", 1);
	}
	lcd_nokia_render(lcd_ptr);
	return;
}

void write_player_turn(LcdNokia *lcd_ptr, bool x_turn){
	lcd_nokia_set_cursor(lcd_ptr, 0, 0);
	if (x_turn){
		lcd_nokia_write_string(lcd_ptr, "    X's Turn  ", 1);
	} else {
		lcd_nokia_write_string(lcd_ptr, "    O's Turn  ", 1);
	}
	lcd_nokia_render(lcd_ptr);
	return;
}

void keypad_init(){
	// Enable Pull-Up Resistors
	MCUCR |= (0 << PUD);
	
	// Configure PIND[4-7] as input
	DDRD = 0x00;
		
	// Enable the internal pull-ups
	PORTD |= 0xF0;
	
	// Set PC4-6 as outputs
	// Drive PC4, 5, 6 Low
	DDRC = 0x70; 
	PORTC = 0x00;
	
	return;
}

uint8_t getKeyPressed()
{
	// Debounce - wait 10 MS
	_delay_ms(10);
	uint8_t c;
	
	for (c = 0; c<3; c++) {	
		//Drive a single col pin low
		PORTC = 0x70 && (0<<(c+4));
		
		if (bit_is_clear(PORTD, 4)) {
			// Row 1 - PD4
			return (uint8_t)(3*0)+(c+1);
			
		} else if (bit_is_clear(PORTD, 5)){
			// Row 2 - PD5
			return (uint8_t)(3*1)+(c+1);
			
		} else if (bit_is_clear(PORTD, 6)) {
			// Row 3 - PD6
			return (uint8_t)(3*2)+(c+1);
			
		} else if (bit_is_clear(PORTD, 7)){
			// Row 4 - PD7
			return (uint8_t)(3*3)+(c+1);
		} else {
				return 0xFF; // no key pressed
		}
	}
	return 0xFF; // no key pressed
}
