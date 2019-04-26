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
//#include "SPI_Master_H_file.h"
//#include "Font.h"
#include "nlcd.h"

int main(void)
{
	struct lcd_nokia lcd;

	lcd_nokia_init();
	lcd_nokia_clear(&lcd);
	lcd_nokia_write_string(&lcd, "IT'S WORKING!",1);
	lcd_nokia_set_cursor(&lcd, 0, 10);
	lcd_nokia_write_string(&lcd, "Nice!", 3);
	lcd_nokia_render(&lcd);

	for (;;) {
		_delay_ms(1000);
	}
}
