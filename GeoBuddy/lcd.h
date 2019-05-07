/*
EE459 Spring 2019
Team 15
Project name: GeoBuddy
Ling Ye
Brian Suitt
Yi Sui
*/

/*
lcd.h and lcd.c handle the communication between the microcontroller and 
the LCD display and also the drawing part
*/

#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h> 
#include <stdlib.h>
#include "gfxfont.h"
#include "glcdfont.c"
#include <math.h>
#include <string.h>


#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
 #define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
 #define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

GFXfont  *gfxFont;

void draw_box(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
void setAddrWindow(uint16_t, uint16_t, uint16_t, uint16_t);
void regout32(uint8_t, uint32_t);
void regout16(uint16_t, uint16_t);
void regout8(uint8_t, uint8_t);
void lcdout(uint8_t);
void lcd_init(void);
void reset();
uint16_t color565(uint8_t, uint8_t, uint8_t);

void drawPixel(int16_t x, int16_t y, uint16_t color);
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint8_t size);
void drawString(char* str, int size, int16_t x, int16_t y, uint16_t color, uint8_t font_size);
void drawParagragh(char* str, int size, uint16_t color);

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void drawHLine(int16_t x, int16_t y, int16_t length, uint16_t color);
void drawVLine(int16_t x, int16_t y, int16_t length,uint16_t color);
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void drawDirectionArrow(uint16_t curr_direction, uint16_t color);

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
#define LCD_CS      (1 << PC3)  // PORTB, bit 2 - LCD /CE (chip enable)
#define LCD_WR      (1 << PC1)  // PORTB, bit 5 - LCD /WR (write)
#define LCD_RST     (1 << PC6)  // PORTB, bit 7 - LCD /RST (reset)
#define LCD_Ctrl_B  (LCD_CD|LCD_RD|LCD_CS|LCD_WR|LCD_RST)

// #define LCD_CD_Command    PORTB &= ~LCD_CD
// #define LCD_CD_Data       PORTB |= LCD_CD
// #define LCD_RD_Active     PORTB &= ~LCD_RD
// #define LCD_RD_Negate     PORTB |= LCD_RD
// #define LCD_CS_Active     PORTB &= ~LCD_CS
// #define LCD_CS_Negate     PORTB |= LCD_CS
// #define LCD_WR_Active     PORTB &= ~LCD_WR
// #define LCD_WR_Negate     PORTB |= LCD_WR
// #define LCD_RST_Active    PORTB &= ~LCD_RST
// #define LCD_RST_Negate    PORTB |= LCD_RST

#define LCD_CD_Command    PORTC &= ~LCD_CD
#define LCD_CD_Data       PORTC |= LCD_CD
#define LCD_RD_Active     PORTC &= ~LCD_RD
#define LCD_RD_Negate     PORTC |= LCD_RD
#define LCD_CS_Active     PORTC &= ~LCD_CS
#define LCD_CS_Negate     PORTC |= LCD_CS
#define LCD_WR_Active     PORTC &= ~LCD_WR
#define LCD_WR_Negate     PORTC |= LCD_WR
#define LCD_RST_Active    PORTC &= ~LCD_RST
#define LCD_RST_Negate    PORTC |= LCD_RST

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

#endif