
// This is used to test the graphic LCD output functionality

#include "Adafruit_GFX.h"
#include "Adafruit_TFTLCD.h"


// Human readable color definition
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


// Need to initialize within the both libaries
void full_initialization()
{
	// some duplicate statement 
	init_GFX(TFTWIDTH,TFTHEIGHT);
	init_board();
}

void test_program()
{
	
}