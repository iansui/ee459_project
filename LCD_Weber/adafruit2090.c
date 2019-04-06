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

void color_bars();
void boxes();
void draw_box(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
void setAddrWindow(uint16_t, uint16_t, uint16_t, uint16_t);
void regout32(uint8_t, uint32_t);
void regout16(uint16_t, uint16_t);
void regout8(uint8_t, uint8_t);
void lcdout(uint8_t);
void initialize(void);
void reset();
uint16_t color565(uint8_t, uint8_t, uint8_t);

/*
  This LCD is set up as a 240 pixel wide (x) by 320 pixel high (y)
  display.  Display position (0,0) is in the upper left.  Writing into
  consecutive locations will write L->R, T->B.
*/
#define LCD_Width   240
#define LCD_Height  320

// Define some bits in the I/O ports

#define LCD_Data_B  0x03        // Bits in Port B for LCD data bits 0-1
#define LCD_Data_D  0xfc        // Bits in Port D for LCD data bits 3-7

// #define LCD_CD      (1 << PB4)  // PORTB, bit 4 - LCD register select (0 = command (addr), 1 = data)
// #define LCD_RD      (1 << PB3)  // PORTB, bit 3 - LCD /RD (read)
// #define LCD_CS      (1 << PB2)  // PORTB, bit 2 - LCD /CE (chip enable)
// #define LCD_WR      (1 << PB5)  // PORTB, bit 5 - LCD /WR (write)
// #define LCD_RST     (1 << PB7)  // PORTB, bit 7 - LCD /RST (reset)
// #define LCD_Ctrl_B  (LCD_CD|LCD_RD|LCD_CS|LCD_WR|LCD_RST)

#define LCD_CD      (1 << PC2)  // PORTB, bit 4 - LCD register select (0 = command (addr), 1 = data)
#define LCD_RD      (1 << PC0)  // PORTB, bit 3 - LCD /RD (read)
#define LCD_CS      (1 << PB2)  // PORTB, bit 2 - LCD /CE (chip enable)
#define LCD_WR      (1 << PB5)  // PORTB, bit 5 - LCD /WR (write)
#define LCD_RST     (1 << PB7)  // PORTB, bit 7 - LCD /RST (reset)
#define LCD_Ctrl_B  (LCD_CD|LCD_RD|LCD_CS|LCD_WR|LCD_RST)

#define LCD_CD_Command    PORTB &= ~LCD_CD
#define LCD_CD_Data       PORTB |= LCD_CD
#define LCD_RD_Active     PORTB &= ~LCD_RD
#define LCD_RD_Negate     PORTB |= LCD_RD
#define LCD_CS_Active     PORTB &= ~LCD_CS
#define LCD_CS_Negate     PORTB |= LCD_CS
#define LCD_WR_Active     PORTB &= ~LCD_WR
#define LCD_WR_Negate     PORTB |= LCD_WR
#define LCD_RST_Active    PORTB &= ~LCD_RST
#define LCD_RST_Negate    PORTB |= LCD_RST

// Define names for the registers (from registers.h)

#define ILI9341_SOFTRESET       0x01
#define ILI9341_SLEEPIN         0x10
#define ILI9341_SLEEPOUT        0x11
#define ILI9341_NORMALDISP      0x13
#define ILI9341_INVERTOFF       0x20
#define ILI9341_INVERTON        0x21
#define ILI9341_GAMMASET        0x26
#define ILI9341_DISPLAYOFF      0x28
#define ILI9341_DISPLAYON       0x29
#define ILI9341_COLADDRSET      0x2A
#define ILI9341_PAGEADDRSET     0x2B
#define ILI9341_MEMORYWRITE     0x2C
#define ILI9341_PIXELFORMAT     0x3A
#define ILI9341_FRAMECONTROL    0xB1
#define ILI9341_DISPLAYFUNC     0xB6
#define ILI9341_ENTRYMODE       0xB7
#define ILI9341_POWERCONTROL1   0xC0
#define ILI9341_POWERCONTROL2   0xC1
#define ILI9341_VCOMCONTROL1    0xC5
#define ILI9341_VCOMCONTROL2    0xC7
#define ILI9341_MEMCONTROL      0x36
#define ILI9341_MADCTL          0x36

#define ILI9341_MADCTL_MY       0x80
#define ILI9341_MADCTL_MX       0x40
#define ILI9341_MADCTL_MV       0x20
#define ILI9341_MADCTL_ML       0x10
#define ILI9341_MADCTL_RGB      0x00
#define ILI9341_MADCTL_BGR      0x08
#define ILI9341_MADCTL_MH       0x04

// Define RGB values for eight color bars

uint8_t red[8] = { 0, 0xff, 0, 0, 0xff, 0, 0xff, 0xff };
uint8_t grn[8] = { 0, 0, 0xff, 0, 0xff, 0xff, 0, 0xff };
uint8_t blu[8] = { 0, 0, 0, 0xff, 0, 0xff, 0xff, 0xff };


int main(void) {
    DDRB |= LCD_Data_B;         // Set PORTB bits 0-1 for output
    DDRD |= LCD_Data_D;         // Set PORTD bits 2-7 for output

    PORTB |= LCD_Ctrl_B;        // Set all the control lines high
    DDRB |= LCD_Ctrl_B;         // Set control port bits for output

    initialize();               // Initialize the LCD display
    
    // Loop between displaying color bars and some boxes
    while (1) {
        color_bars();
        _delay_ms(1000);

	boxes();
	_delay_ms(1000);
    }
}

/*
  color_bars - Draw eight color bars on the full screen 
*/
void color_bars()
{
    uint8_t i, j, k;
    uint16_t color;
    uint16_t y1, y2;
    uint8_t hi, lo;

    for (j = 0; j < 8; j++) {
	y1  = j * 40;                  // Row number of start of bar
	y2 = y1 + 39;                  // Row number of end of bar
	// Set window to a 240x40 strip from (0,y1) to (239,y2)
	setAddrWindow(0, y1, LCD_Width-1, y2);

	LCD_CS_Active;

	LCD_CD_Command;
	lcdout(ILI9341_MEMORYWRITE);        // Write to memory command

	LCD_CD_Data;
        color = color565(red[j], grn[j], blu[j]);
        hi = color >> 8;
        lo = color;

        for (k = 0; k < 40; k++) {      // 40 rows in each bar
            for (i = 0; i < 240; i++) { // 240 pixels in each row
                lcdout(hi);
                lcdout(lo);
            }
        }

	LCD_CS_Negate;
    }
}

/*
  boxes - Draws some boxes on the screen with different fill colors
*/
void boxes()
{
    draw_box(0, 0, LCD_Width-1, LCD_Height-1, color565(255,255,255));
    draw_box(20, 30, 120, 200, color565(30, 255, 100));
    draw_box(70, 180, 150, 270, color565(255, 40, 70));
    draw_box(160, 10, 220, 300, color565(100, 10, 255));
    draw_box(80, 50, 170, 120, color565(225, 200, 40));
    draw_box(130, 130, 200, 315, color565(30, 240, 250));
    draw_box(10, 260, 130, 300, color565(240, 30, 250));
    draw_box(100, 100, 140, 140, color565(255, 0, 255));
}

/*
  draw_box - Draw a box from (x1,y1), the upper left, to (x2,y2), the lower right
  and fill it with the color value in "color".
*/
void draw_box(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint32_t n;
    uint8_t hi, lo;

    if (x1 > x2 || y1 > y2)
	return;
    if (x1 < 0 || x2 >= LCD_Width || y1 < 0 || y2 >= LCD_Height)
	return;
    setAddrWindow(x1, y1, x2, y2);

    hi = color >> 8;
    lo = color;

    LCD_CS_Active;

    LCD_CD_Command;
    lcdout(ILI9341_MEMORYWRITE);         // Write to memory command

    LCD_CD_Data;
    n = (uint32_t)(x2 - x1 + 1) * (uint32_t)(y2 - y1 + 1);
    while (n--) {
	lcdout(hi);
	lcdout(lo);
    }
    
    LCD_CS_Negate;
}

/*
  setAddrWindow - Sets the LCD address window.
*/
void setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint32_t w;
 
    w = (uint32_t) x1 << 16;
    w |= x2;
    regout32(ILI9341_COLADDRSET, w);
    w = (uint32_t) y1 << 16;
    w |= y2;
    regout32(ILI9341_PAGEADDRSET, w);
}

/*
  regout32 - Output a 8-bit register address and 32-bit data value to the LCD
*/
void regout32(uint8_t a, uint32_t d)
{
    LCD_CS_Active;              // Enable the chip

    LCD_CD_Command;             // CD = command
    lcdout(a);                  // Send the address

    LCD_CD_Data;                // CD = data
    lcdout(d >> 24);            // Send high part of the data
    lcdout(d >> 16);
    lcdout(d >> 8);
    lcdout(d);                  // Low part of data

    LCD_CS_Negate;              // Disable the chip
}

/*
  regout16 - Output a 16-bit register address and 16-bit data value to the LCD
*/
void regout16(uint16_t a, uint16_t d)
{
    uint8_t hi, lo;

    LCD_CS_Active;              // Enable the chip

    hi = a >> 8;
    lo = a & 0xff;
    LCD_CD_Command;             // CD = command
    lcdout(hi);                 // Send high part of reg address
    lcdout(lo);                 // Send low part of reg address

    hi = d >> 8;
    lo = d & 0xff;
    LCD_CD_Data;                // CD = data
    lcdout(hi);                 // High part of data
    lcdout(lo);                 // Low part of data

    LCD_CS_Negate;              // Disable the chip
}

/*
  regout8 - Output a 8-bit register address and 8-bit data value to the LCD
*/
void regout8(uint8_t a, uint8_t d)
{
    LCD_CS_Active;              // Enable the chip

    LCD_CD_Command;             // CD = command
    lcdout(a);                  // Send register address

    LCD_CD_Data;                // CD = data
    lcdout(d);                  // Send data

    LCD_CS_Negate;              // Disable the chip
}

/*
  lcdout - Write a byte to the LCD
*/
void lcdout(uint8_t x)
{
    PORTB |= x & LCD_Data_B;    // put low two bits in B
    PORTB &= (x | ~LCD_Data_B);
    PORTD |= x & LCD_Data_D;    // put high six bits in D
    PORTD &= (x | ~LCD_Data_D);
    LCD_WR_Active;              // Toggle the /WR input
    LCD_WR_Negate;
}

/*
  initialize - Do various things to initialize the LCD.
*/
void initialize()
{
    reset();

    regout8(ILI9341_SOFTRESET, 0);
    _delay_ms(50);
    regout8(ILI9341_DISPLAYOFF, 0);

    regout8(ILI9341_POWERCONTROL1, 0x23);
    regout8(ILI9341_POWERCONTROL2, 0x10);
    regout16(ILI9341_VCOMCONTROL1, 0x2B2B);
    regout8(ILI9341_VCOMCONTROL2, 0xC0);
    regout8(ILI9341_MEMCONTROL, ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
    regout8(ILI9341_PIXELFORMAT, 0x55);
    regout16(ILI9341_FRAMECONTROL, 0x001B);

    regout8(ILI9341_ENTRYMODE, 0x07);

    regout8(ILI9341_SLEEPOUT, 0);
    _delay_ms(150);
    regout8(ILI9341_DISPLAYON, 0);
    _delay_ms(500);
}

/*
  reset - Force a reset of the LCD
*/
void reset()
{
    uint8_t i;

    PORTB |= LCD_Ctrl_B;        // Set all the control lines high

    _delay_ms(100);     // Delay 100ms to let things settle

    LCD_RST_Active;
    _delay_us(2);      // Extend the reset a bit
    LCD_RST_Negate;

    // Not sure why this is needed, or if it is.
    LCD_CS_Active;
    LCD_CD_Command;
    lcdout(0);
    for (i = 3; i != 0; i--) {
        LCD_WR_Active;
        LCD_WR_Negate;
    }
    LCD_CS_Negate;

    _delay_ms(100);
}

/*
  color565 - Change three R, G, B byte values into a 16-bit color value
*/
uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3);
}
