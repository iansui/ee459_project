/*************************************************************
*       adafruit2090 - Demonstrate interface to a graphics LCD display
*
*       This program will print a message on an Adafruit 2090
*       graphics LCD (ILI9341 controller).  Most of it is based on the
*       sample code provided by Adafruit in Adafruit_TFTLCD.cpp and
*       associated files.
*
*       Port B, bit 4 (0x10) - output to C/D (cmd/data) input of display
*               bit 3 (0x08) - output to /RD (read) input of display
*               bit 2 (0x04) - output to /CS (chip select) input of display
*               bit 5 (0x20) - output to /WR (write) input of display
*               bit 7 (0x80) - output to /RST (reset) input of display
*       Port B, bits 0-1, Port D, bits 2-7 - DB0-DB7 of display.
*
* Revision History
* Date Author Description
* 04/12/13 A. Weber    Initial release for Adafruit 335
* 04/12/14 A. Weber    Modified for Adafruit 1770
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "draw.h"

int main(void) {
    DDRB |= LCD_Data_B;         // Set PORTB bits 0-1 for output
    DDRD |= LCD_Data_D;         // Set PORTD bits 2-7 for output

    PORTC |= LCD_Ctrl_B;        // Set all the control lines high
    DDRC |= LCD_Ctrl_B;         // Set control port bits for output

    initialize();               // Initialize the LCD display
    
    // Loop between displaying color bars and some boxes
    while (1) {
        //color_bars();
        // for(int i = 0; i < 100; i++){
        //   for(int j = 0; j < 100; j++){
        //     drawPixel(i, j, color565(30, 255, 100));
        //   }
        // }

        drawchar(100, 100, 'L', color565(30, 255, 100), 1);
        _delay_ms(1000);


	//boxes();
	//_delay_ms(1000);
    }
}


