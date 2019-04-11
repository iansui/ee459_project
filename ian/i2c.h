
#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

# define FOSC 9830400
# define BDIV ( FOSC / 100000 - 16) / 2 + 1

void i2c_init(uint8_t bdiv);

#endif