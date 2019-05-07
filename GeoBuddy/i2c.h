/*
EE459 Spring 2019
Team 15
Project name: GeoBuddy
Ling Ye
Brian Suitt
Yi Sui
*/

/*
i2c.h and i2c.c handle the initiazlization of i2c on the microcontrolelr,
and the communication between the microcontroller and the touch data
register in the LCD module
*/

#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define FOSC 9830400
#define BDIV ( FOSC / 100000 - 16) / 2 + 1

uint8_t touch_threshold;
uint8_t touch_address;
uint8_t touch_reg_numtouches;

uint8_t touches;
uint16_t touchX[2], touchY[2];

void i2c_init(uint8_t bdiv);
uint8_t i2c_io(uint8_t device_addr, uint8_t *ap, uint16_t an, 
               uint8_t *wp, uint16_t wn, uint8_t *rp, uint16_t rn);
bool touch_init();
void update_touch();

#endif