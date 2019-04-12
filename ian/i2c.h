
#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stddef.h>

#define FOSC 9830400
#define BDIV ( FOSC / 100000 - 16) / 2 + 1

#define TOUCH_DEVICE_ADDR 0x70
#define TOUCH_REG_NUMTOUCHES 0x02
#define FT62XX_REG_VENDID 0xA8
#define FT62XX_DEFAULT_THRESHOLD 128

void i2c_init(uint8_t bdiv);
uint8_t i2c_io(uint8_t device_addr, uint8_t *ap, uint16_t an, 
               uint8_t *wp, uint16_t wn, uint8_t *rp, uint16_t rn);


#endif