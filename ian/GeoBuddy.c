//sudo picocom --baud 9600 /dev/ttyUSB0

/*Inclusions*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <util/delay.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "serial.h"
#include "lcd.h"
#include "GeoBuddy.h"
#include "i2c.h"


int main(void){

	//initialize the LCD
	DDRB |= LCD_Data_B;         // Set PORTB bits 0-1 for output
    DDRD |= LCD_Data_D;         // Set PORTD bits 2-7 for output
    PORTC |= LCD_Ctrl_B;        // Set all the control lines high
    DDRC |= LCD_Ctrl_B;         // Set control port bits for output
    lcd_init();               // Initialize the LCD display
	background_color = color565(255,255,255);
	background_color_test = color565(0,0,255);
	text_color = color565(0, 0, 0);
	arrow_color = color565(255, 0, 0);
	
	//initialize serial connection
	serial_init();

	// Initialize the serial and gps interface
	gps_common_init();
	int gps_iteration_count = 0;

	// initialize i2c port
	i2c_init(BDIV);

	// initialize touch
	touch_init();	

	
	//draw background
	draw_box(0, 0, LCD_Width-1, LCD_Height-1, background_color_test);

	//start infinite loop
	while(1){

		update_touch();
		snprintf(lcd_output_buf, sizeof(lcd_output_buf), "num of touches: %i", touches);
		draw_box(10, 10, 200, 30, background_color);
		drawString(lcd_output_buf, 50, 12, 12, text_color);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

		snprintf(lcd_output_buf, sizeof(lcd_output_buf), "point0: %i %i", touchX[0], touchY[0]);
		draw_box(10, 30, 200, 50, background_color);
		drawString(lcd_output_buf, 50, 12, 32, text_color);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

		snprintf(lcd_output_buf, sizeof(lcd_output_buf), "point1: %i %i", touchX[1], touchY[1]);
		draw_box(10, 50, 200, 70, background_color);
		drawString(lcd_output_buf, 50, 12, 52, text_color);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

		_delay_ms(50);


	}
}