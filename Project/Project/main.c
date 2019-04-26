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
#include "nlcd.h"

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

int main(void)
{
	struct lcd_nokia lcd;

	lcd_nokia_init();
	board_int(&lcd);
	
	// Example
	write_token(&lcd, 1, COL0, ROW2);
	_delay_ms(1000);
	write_token(&lcd, 0, COL1, ROW2);
	_delay_ms(1000);
	write_token(&lcd, 1, COL0, ROW0);
	_delay_ms(1000);
	write_token(&lcd, 0, COL2, ROW2);
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

void write_token(LcdNokia *lcd_ptr, bool is_x, uint8_t col, uint8_t row){
	lcd_nokia_set_cursor(lcd_ptr, col, row);
	if(is_x){
		lcd_nokia_write_string(lcd_ptr, "X", 1);
	} else {
		lcd_nokia_write_string(lcd_ptr, "O", 1);
	}
	lcd_nokia_render(lcd_ptr);
	return;
}