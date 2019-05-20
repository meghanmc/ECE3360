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
#include "nlcd.h"		// library for displaying to Nokia 5110 LCD - from https://github.com/ss2222/AVR-nokia-5110

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
void pin_change_init();
uint8_t getKeyPressed();

// Global Values
uint8_t key_val = 0xFF;
int board[9] = {-1,-1,-1,-1,-1,-1,-1,-1,-1};
bool x_turn = true;

ISR(PCINT2_vect){
	// Decode keypress
	key_val = getKeyPressed();
	// Set PORTC back to low
	PORTC = 0x00;
}

int main(void)
{
// Program Initialization
	struct lcd_nokia lcd;
	// Board is initialized as -1's. When tokens are placed, 0 = O token, 1 is X token
	_delay_ms(500); // give the LCD time to power up
	lcd_nokia_init();
	lcd_nokia_clear(&lcd);
	board_int(&lcd);
	keypad_init();
	pin_change_init();
	
// End Initialization

// Begin Game Logic

	while(1){
		if (key_val == 0x01){
			// Pressed Row 1 - RESET/ NEW GAME
			for (int i =0; i <9 ;i++){
				board[i] = -1;
			}
			x_turn = true;
			lcd_nokia_clear(&lcd);
			lcd_nokia_set_cursor(&lcd, 0, 0);
			lcd_nokia_write_string(&lcd, "   New Game   ", 1);
			lcd_nokia_render(&lcd);
			_delay_ms(1000);
			board_int(&lcd);

		}  else if (key_val == 0x04){
			// Pressed "4" i.e. Row 0 Col 0;
			if (board[0] == -1){
				board[0] = x_turn;
				write_token(&lcd, x_turn, COL0, ROW0);
				x_turn = !x_turn;
				write_player_turn(&lcd, x_turn);
				lcd_nokia_render(&lcd);
			}

		} else if (key_val == 0x05){
			// Pressed "5" i.e. Row 0 Col 1
			if(board[1] == -1){
				board[1]= x_turn;
				write_token(&lcd, x_turn, COL1, ROW0);
				x_turn = !x_turn;
				write_player_turn(&lcd, x_turn);
				lcd_nokia_render(&lcd);
			}

		} else if (key_val == 0x06){
			// Pressed "6" i.e. Row 0 Col 2
			if(board[2]==-1){
				board[2]=x_turn;
				write_token(&lcd, x_turn, COL2, ROW0);
				x_turn = !x_turn;
				write_player_turn(&lcd, x_turn);
				lcd_nokia_render(&lcd);
			}

		} else if (key_val == 0x07){
			// Pressed "7" i.e. Row 1 Col 0
			if(board[3]==-1){
				board[3]=x_turn;
				write_token(&lcd, x_turn, COL0, ROW1);
				x_turn = !x_turn;
				write_player_turn(&lcd, x_turn);
				lcd_nokia_render(&lcd);
			}

		} else if (key_val == 0x08){
			// Pressed "8" i.e. Row 1 Col 1
			if(board[4]==-1){
				board[4]=x_turn;
				write_token(&lcd, x_turn, COL1, ROW1);
				x_turn = !x_turn;
				write_player_turn(&lcd, x_turn);
				lcd_nokia_render(&lcd);
			}

		} else if (key_val == 0x09){
			// Pressed "9" i.e. Row 1 Col 2
			if(board[5]==-1){
				board[5]=x_turn;
				write_token(&lcd, x_turn, COL2, ROW1);
				x_turn = !x_turn;
				write_player_turn(&lcd, x_turn);
				lcd_nokia_render(&lcd);
			}

		} else if (key_val == 0x0A){
			// Pressed "*" i.e. Row 2 Col 0
			if(board[6]==-1){
				board[6]=x_turn;
				write_token(&lcd, x_turn, COL0, ROW2);
				x_turn = !x_turn;
				write_player_turn(&lcd, x_turn);
				lcd_nokia_render(&lcd);
			}

		} else if (key_val == 0x0B){
			// Pressed "0" i.e. Row 2 Col 2
			if(board[7]==-1){
				board[7]=x_turn;
				write_token(&lcd, x_turn, COL1, ROW2);
				x_turn = !x_turn;
				write_player_turn(&lcd, x_turn);
				lcd_nokia_render(&lcd);
			}

		} else if (key_val == 0x0C){
			// Pressed "#" i.e. Row 2 Col 2
			if(board[8]==-1){
				board[8]=x_turn;
				write_token(&lcd, x_turn, COL2, ROW2);
				x_turn = !x_turn;
				write_player_turn(&lcd, x_turn);
				lcd_nokia_render(&lcd);
			}
		} 
		
		// Check for a winner!
		// Possible Combinations 012 345 678 036 147 258 048 246
		bool winner = false;
		if((board[0] != -1) && (board[0] == board[1]) && (board[1] == board[2])){
			winner = true;
		} else if((board[6] != -1) && (board[6] == board[7]) && (board[7] == board[8])){
			winner = true;
		} else if ((board[3] != -1) && (board[3] == board[4]) && (board[4] == board[5])){
			winner = true;
		} else if((board[0] != -1) && (board[0] == board[3]) && (board[3] == board[6])){
			winner = true;
		} else if((board[1] != -1) && (board[1] == board[4]) && (board[4] == board[7])){
			winner = true;
		} else if((board[2] != -1) && (board[2] == board[5]) && (board[5] == board[8])){
			winner = true;
		} else if((board[0] != -1) && (board[0] == board[4]) && (board[4] == board[8])){
			winner = true;
		}  else if((board[2] != -1) && (board[2] == board[4]) && (board[4] == board[6])){
			winner = true;
		}
		
		if(winner){
			lcd_nokia_clear(&lcd);
			lcd_nokia_set_cursor(&lcd, 0, 0);
			if(x_turn){
				lcd_nokia_write_string(&lcd, " O Wins!", 2);
			} else {
				lcd_nokia_write_string(&lcd, " X Wins!", 2);
			}
			lcd_nokia_set_cursor(&lcd, 0, ROW1+8);
			lcd_nokia_write_string(&lcd, "Press New Game to continue", 1);
			lcd_nokia_render(&lcd);
			for (int i =0; i <9 ;i++){
				board[i] = -1;
			}
			
		}

		// reset key to invalid value
		key_val = 0xFF;

	}
// End Game Logics
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
	//MCUCR |= (0 << PUD);
	MCUCR &= ~(1<<PUD);
	
	// Configure PIND[4-7] as input
	DDRD = 0x00;
		
	// Enable the internal pull-ups by writing logic 1 to 
	PORTD |= 0xFF;
	
	// Update: Set PC 0-2 as outputs
	// Drive PC0-2 low
	DDRC = 0x07;
	PORTC = 0x00;
	
	return;
}

void pin_change_init(){

	//Set Pin Change Masks for PCINT20-PCINT23
	PCMSK2 |= ((1<<PCINT20) | (1<<PCINT21) | (1<<PCINT22) | (1<<PCINT23));

	//Set Pin Change Control Register -  Pin Change Interrupt Enable 2
	PCICR |= (1<<PCIE2);

	//Set global interrupts
	sei();
}

uint8_t getKeyPressed()
{
	// Debounce - wait 10 MS
	_delay_ms(10);
	uint8_t c ;
	
	for (c = 0; c<3; c++) {	
		//Drive a single col pin low
		PORTC = 0x07 & (~(1<<c));
		//c=0	00000111 & (~0000001) = 00000111 & (1111110) = 00000110
		//c=1	00000111 & (~0000010) = 00000111 & (1111101) = 00000101
		//c=2	00000111 & (~0000100) = 00000111 & (1111011) = 00000011

		// Changed all of these from PORTD to PIND
		if (bit_is_clear(PIND, 4)) {
			// Row 1 - PD4
			return (uint8_t)(3*0)+(c+1);
			// Row 1 is always returning 1
			
		} else if (bit_is_clear(PIND, 5)){
			// Row 2 - PD5
			return (uint8_t)(3*1)+(c+1);
						
		} else if (bit_is_clear(PIND, 6)) {
			// Row 3 - PD6
			return (uint8_t)(3*2)+(c+1);
			
		} else if (bit_is_clear(PIND, 7)){
			// Row 4 - PD7
			return (uint8_t)(3*3)+(c+1);
		}
	}
	return 0xFF; // no key pressed
}