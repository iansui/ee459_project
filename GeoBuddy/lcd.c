#include "lcd.h"

uint8_t red[8] = { 0, 0xff, 0, 0, 0xff, 0, 0xff, 0xff };
uint8_t grn[8] = { 0, 0, 0xff, 0, 0xff, 0xff, 0, 0xff };
uint8_t blu[8] = { 0, 0, 0, 0xff, 0, 0xff, 0xff, 0xff };

//  color_bars - Draw eight color bars on the full screen 
/*
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
*/



//  boxes - Draws some boxes on the screen with different fill colors
/*
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
*/

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
void lcd_init()
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

    PORTC |= LCD_Ctrl_B;        // Set all the control lines high

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


/*
draw a pixel
*/
void drawPixel(int16_t x, int16_t y, uint16_t color) {
  // Clip
  if((x < 0) || (y < 0) || (x >= LCD_Width) || (y >= LCD_Height)) return;

    setAddrWindow(x, y, LCD_Width-1, LCD_Height-1);
    LCD_CS_Active;
    LCD_CD_Command;
    lcdout(ILI9341_MEMORYWRITE);
    lcdout(0x2C);
    LCD_CD_Data;
    lcdout(color >> 8); 
    lcdout(color);
    LCD_CS_Negate;
}

/*
draw a char
*/
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint8_t size)
{
    // testing if the x,y is out of bound and whether the size of word can fit in
    if((x >= LCD_Width) || (y >= LCD_Height) || 
        ((x + 6 * size - 1) < 0) ||((y + 8 * size - 1) < 0))
    return;

    for(int8_t i=0; i<5; i++ ) 
    { 
        uint8_t line = pgm_read_byte(&font[c * 5 + i]);
        for(int8_t j=0; j<8; j++, line >>= 1) 
        {
            if(line & 1) {
                if(size == 1){
                    drawPixel(x+i, y+j, color);
                } 
                else{
                    draw_box(x+i*size, y+j*size, x+i*size+size-1, y+j*size+size-1, color);
                }
            }
        }
    }
}

/*
draw a string with the first chat at (x, y)
*/

void drawString(char* str, int size, int16_t x, int16_t y, uint16_t color, uint8_t font_size){
    // testing if the x,y is out of bound and whether the size of word can fit in
    if((x >= LCD_Width) || (y >= LCD_Height) || 
        ((x + 6 * size - 1) < 0) ||((y + 8 * size - 1) < 0))
    return;

    for(int i = 0; i < size; i++){
      if(str[i] == '\0'){
        break;
      }
      drawChar(x+(i*(6*font_size)), y, str[i], color, font_size);
    }
}

void drawParagragh(char* str, int size, uint16_t color){
    
    char line[37];
    int counter = 0;
    int line_y = 110;
    while(1){
        if(size <= 36 || (counter + 36 > size)){
            strncpy(line, &str[counter], sizeof(line)-1);
            line[36] = '\0';
            drawString(line, sizeof(line), 12, line_y, color, 1);
            return;
        }
        else{
            strncpy(line, &str[counter], sizeof(line)-1);
            line[36] = '\0';
            drawString(line, sizeof(line), 12, line_y, color, 1);
            memset(line, 0, sizeof(line));
            counter += (sizeof(line)-1);
            line_y += 12;
        }
    }

}


/*
    draw a line from (x0, y0) to (x1, y1)
*/

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            drawPixel(y0, x0, color);
        } else {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}


/*
draw a vertical line from starting point(x,y)
*/
void drawVLine(int16_t x, int16_t y,int16_t length, uint16_t color) {
    drawLine(x, y, x, y+length-1, color);
}


/*
draw a vertical line from starting point(x,y)
*/
void drawHLine(int16_t x, int16_t y,int16_t length, uint16_t color) {
    drawLine(x, y, x+length-1, y, color);
}

void drawDirectionArrow(uint16_t direction, uint16_t color){
    //draw N arrow
    if(direction == 2){
        draw_box(110, 130, 130, 170, color);
        drawTriangle(100, 130, 120, 110, 140, 130, color);
    }
    //draw S arrow
    if(direction == 6){
        draw_box(110, 130, 130, 170, color);
        drawTriangle(100, 170, 120, 190, 140, 170, color);
    }
    //draw E arrow
    if(direction == 0){
        draw_box(100, 140, 140, 160, color);
        drawTriangle(160, 150, 140, 130, 140, 170, color);
    }

    //draw W arrow
    if(direction == 4){
        draw_box(100, 140, 140, 160, color);
        drawTriangle(80, 150, 100, 130, 100, 170, color);
    }

    //draw NE arrow
    if(direction == 1){

        // double theta = (M_PI/4)*(-1);
        // int16_t x0 = (140-120)*cos(theta) - (140-150)*sin(theta) + 120;
        // int16_t y0 = (140-120)*sin(theta) + (140-150)*cos(theta) + 150;
        // int16_t x1 = (140-120)*cos(theta) - (160-150)*sin(theta) + 120;
        // int16_t y1 = (140-120)*sin(theta) + (160-150)*cos(theta) + 150;
        // int16_t x2 = (100-120)*cos(theta) - (160-150)*sin(theta) + 120;
        // int16_t y2 = (100-120)*sin(theta) + (160-150)*cos(theta) + 150;
        // int16_t x3 = (100-120)*cos(theta) - (140-150)*sin(theta) + 120;
        // int16_t y3 = (100-120)*sin(theta) + (140-150)*cos(theta) + 150;
        // int16_t x4 = (160-120)*cos(theta) - (150-150)*sin(theta) + 120;
        // int16_t y4 = (160-120)*sin(theta) + (150-150)*cos(theta) + 150;
        // int16_t x5 = (140-120)*cos(theta) - (130-150)*sin(theta) + 120;
        // int16_t y5 = (140-120)*sin(theta) + (130-150)*cos(theta) + 150;
        // int16_t x6 = (140-120)*cos(theta) - (170-150)*sin(theta) + 120;
        // int16_t y6 = (140-120)*sin(theta) + (170-150)*cos(theta) + 150;
        // drawTriangle(x3, y3, x1, y1, x0, y0, color);
        // drawTriangle(x3, y3, x1, y1, x2, y2, color);
        // drawTriangle(x4, y4, x5, y5, x6, y6, color);

        drawTriangle(98, 157, 141, 142, 127, 128, color);
        drawTriangle(98, 157, 142, 142, 112, 171, color);
        drawTriangle(148, 121, 120, 121, 148, 150, color);
    }

    //draw SW arrow
    if(direction == 5){
        // double theta = (M_PI/4)*(-1);
        // int16_t x0 = (140-120)*cos(theta) - (140-150)*sin(theta) + 120;
        // int16_t y0 = (140-120)*sin(theta) + (140-150)*cos(theta) + 150;
        // int16_t x1 = (140-120)*cos(theta) - (160-150)*sin(theta) + 120;
        // int16_t y1 = (140-120)*sin(theta) + (160-150)*cos(theta) + 150;
        // int16_t x2 = (100-120)*cos(theta) - (160-150)*sin(theta) + 120;
        // int16_t y2 = (100-120)*sin(theta) + (160-150)*cos(theta) + 150;
        // int16_t x3 = (100-120)*cos(theta) - (140-150)*sin(theta) + 120;
        // int16_t y3 = (100-120)*sin(theta) + (140-150)*cos(theta) + 150;
        // int16_t x4 = (80-120)*cos(theta) - (150-150)*sin(theta) + 120;
        // int16_t y4 = (80-120)*sin(theta) + (150-150)*cos(theta) + 150;
        // int16_t x5 = (100-120)*cos(theta) - (130-150)*sin(theta) + 120;
        // int16_t y5 = (100-120)*sin(theta) + (130-150)*cos(theta) + 150;
        // int16_t x6 = (100-120)*cos(theta) - (170-150)*sin(theta) + 120;
        // int16_t y6 = (100-120)*sin(theta) + (170-150)*cos(theta) + 150;
        // drawTriangle(x3, y3, x1, y1, x0, y0, color);
        // drawTriangle(x3, y3, x1, y1, x2, y2, color);
        // drawTriangle(x4, y4, x5, y5, x6, y6, color);

        drawTriangle(98, 157, 141, 142, 127, 128, color);
        drawTriangle(98, 157, 141, 142, 112, 171, color);
        drawTriangle(91, 178, 91, 150, 120, 178, color);
    }

    //draw NW arrow
    if(direction == 3){

        // double theta = M_PI/4;
        // int16_t x0 = (140-120)*cos(theta) - (140-150)*sin(theta) + 120;
        // int16_t y0 = (140-120)*sin(theta) + (140-150)*cos(theta) + 150;
        // int16_t x1 = (140-120)*cos(theta) - (160-150)*sin(theta) + 120;
        // int16_t y1 = (140-120)*sin(theta) + (160-150)*cos(theta) + 150;
        // int16_t x2 = (100-120)*cos(theta) - (160-150)*sin(theta) + 120;
        // int16_t y2 = (100-120)*sin(theta) + (160-150)*cos(theta) + 150;
        // int16_t x3 = (100-120)*cos(theta) - (140-150)*sin(theta) + 120;
        // int16_t y3 = (100-120)*sin(theta) + (140-150)*cos(theta) + 150;
        // int16_t x4 = (80-120)*cos(theta) - (150-150)*sin(theta) + 120;
        // int16_t y4 = (80-120)*sin(theta) + (150-150)*cos(theta) + 150;
        // int16_t x5 = (100-120)*cos(theta) - (130-150)*sin(theta) + 120;
        // int16_t y5 = (100-120)*sin(theta) + (130-150)*cos(theta) + 150;
        // int16_t x6 = (100-120)*cos(theta) - (170-150)*sin(theta) + 120;
        // int16_t y6 = (100-120)*sin(theta) + (170-150)*cos(theta) + 150;
        // drawTriangle(x3, y3, x1, y1, x0, y0, color);
        // drawTriangle(x3, y3, x1, y1, x2, y2, color);
        // drawTriangle(x4, y4, x5, y5, x6, y6, color);

        drawTriangle(112, 128, 127, 171, 141, 157, color);
        drawTriangle(112, 128, 127, 171, 98, 142, color);
        drawTriangle(91, 121, 120, 121, 91, 150, color);
    }

    //draw SE arrow
    if(direction == 7){

        // double theta = M_PI/4;
        // int16_t x0 = (140-120)*cos(theta) - (140-150)*sin(theta) + 120;
        // int16_t y0 = (140-120)*sin(theta) + (140-150)*cos(theta) + 150;
        // int16_t x1 = (140-120)*cos(theta) - (160-150)*sin(theta) + 120;
        // int16_t y1 = (140-120)*sin(theta) + (160-150)*cos(theta) + 150;
        // int16_t x2 = (100-120)*cos(theta) - (160-150)*sin(theta) + 120;
        // int16_t y2 = (100-120)*sin(theta) + (160-150)*cos(theta) + 150;
        // int16_t x3 = (100-120)*cos(theta) - (140-150)*sin(theta) + 120;
        // int16_t y3 = (100-120)*sin(theta) + (140-150)*cos(theta) + 150;
        // int16_t x4 = (160-120)*cos(theta) - (150-150)*sin(theta) + 120;
        // int16_t y4 = (160-120)*sin(theta) + (150-150)*cos(theta) + 150;
        // int16_t x5 = (140-120)*cos(theta) - (130-150)*sin(theta) + 120;
        // int16_t y5 = (140-120)*sin(theta) + (130-150)*cos(theta) + 150;
        // int16_t x6 = (140-120)*cos(theta) - (170-150)*sin(theta) + 120;
        // int16_t y6 = (140-120)*sin(theta) + (170-150)*cos(theta) + 150;
        // drawTriangle(x3, y3, x1, y1, x0, y0, color);
        // drawTriangle(x3, y3, x1, y1, x2, y2, color);
        // drawTriangle(x4, y4, x5, y5, x6, y6, color);

        drawTriangle(112, 128, 127, 171, 141, 157, color);
        drawTriangle(112, 128, 127, 171, 98, 142, color);
        drawTriangle(148, 178, 148, 150, 120, 178, color);
    }
}


/*


void drawHLine(int16_t x, int16_t y, int16_t length, uint16_t color)
{
    int16_t x_length = x+length-1;

    if((length <= 0) ||  (y < 0) || ( y>=LCD_Height) 
    ||(x>= LCD_Width) || ((x+length-1) <0)) 
        return;

    if (x_length >= LCD_Width)
    {
        x_length = LCD_Width-1;
        length = x_length -x+1;
    }
    setAddrWindow(x,y,x2,y);
    flood(color,length);
    LCD_CS_Negate;
}

void drawVLine(int16_t x, int16_t y, int16_t length,uint16_t color)
{
    int16_t y_length = y+length-1;

    if((length <= 0) ||  (y < 0) || ( y>=LCD_Height) 
    ||(x>= LCD_Width) || ((y+length-1) <0)) 
        return;

    if (y_length >= LCD_Height)
    {
        y_length = LCD_Height-1;
        length = y_length -x+1;
    }
    setAddrWindow(x,y,x,y2);
    flood(color,length);
    LCD_CS_Negate;
}


// Block Fill
void flood(uint16_t color, uint32_t len)
{
    uint8_t i;
    LCD_CS_Active;
    LCD_CD_Command;
    lcdout(0x2c);

    // Write the 1st pixel
    LCD_CD_Data;
    lcdout(color >> 8); 
    lcdout(color);
    length = length-1;
    uint16_t blocks = (uint16_t)(length/64);
    if (color == (color <<8))
    {
        while(blocks--)
        {
            i = 16;
            do
            {
                LCD_WR_Active;           LCD_WR_Negate;
                LCD_WR_Active;           LCD_WR_Negate;
                LCD_WR_Active;           LCD_WR_Negate;
                LCD_WR_Active;           LCD_WR_Negate;
                LCD_WR_Active;           LCD_WR_Negate;
                LCD_WR_Active;           LCD_WR_Negate;
                LCD_WR_Active;           LCD_WR_Negate;
                LCD_WR_Active;           LCD_WR_Negate;
                LCD_WR_Active;           LCD_WR_Negate;

            }while(--i);
        }
        
        for(i = (uint8_t)length & 63;i>=0;i--)
        {
            LCD_WR_Active;           LCD_WR_Negate;
            LCD_WR_Active;           LCD_WR_Negate;
        }

    }
    else
    {
        while(blocks--)
        {
            i = 16
            do
            {
                lcdout(color >> 8);     lcdout(color);
                lcdout(color >> 8);     lcdout(color);
                lcdout(color >> 8);     lcdout(color);
                lcdout(color >> 8);     lcdout(color);
                lcdout(color >> 8);     lcdout(color);
                lcdout(color >> 8);     lcdout(color);
                lcdout(color >> 8);     lcdout(color);
                lcdout(color >> 8);     lcdout(color);

            }while(--i);
        }
        for(i = (uint8_t)length & 63;i>=0;i--)
        {
            lcdout(color >> 8);     lcdout(color);
        }

    }

    LCD_CS_Negate;

}

*/

void drawTriangle(int16_t x0, int16_t y0,int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    
   if (y0 > y1) {
       _swap_int16_t(y0,y1);
       _swap_int16_t(x0,x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1); 
        _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1); 
        _swap_int16_t(x0, x1);
    }


    int16_t dx01 = x1 - x0;
    int16_t dy01 = y1 - y0;
    int16_t dx02 = x2 - x0;
    int16_t dy02 = y2 - y0;
    int16_t dx12 = x2 - x1;
    int16_t dy12 = y2 - y1;
    int32_t sa   = 0;
    int32_t sb   = 0;

    int16_t a, b, last, y;
    if(y1 == y2) 
        last = y1;   
    else         
        last = y1-1; 

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;

        if(a > b) _swap_int16_t(a,b);
        drawHLine(a, y, b-a+1, color);
    }

    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if(a > b) _swap_int16_t(a,b);
        drawHLine(a, y, b-a+1, color);
    }


}
