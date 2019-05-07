/*
EE459 Spring 2019
Team 15
Project name: GeoBuddy
Ling Ye
Brian Suitt
Yi Sui
*/

/*
compass.h and compass.c handle the communication between the
microcontroller and the compass chip thru i2c
*/


#ifndef COMPASS_H
#define COMPASS_H

#include <avr/io.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "i2c.h"

uint8_t accel_address;
uint8_t mag_address;
uint8_t reg_accel_ctrl_reg1_a;
uint8_t reg_mag_mr_reg_m;
uint8_t reg_mag_cra_reg_m;
uint8_t reg_mag_crb_reg_m;

float _lsm303Mag_Gauss_LSB_XY;
float _lsm303Mag_Gauss_LSB_Z;

int16_t raw_x;
int16_t raw_y;
int16_t raw_z;

float mag_x;
float mag_y;
float mag_z;

uint8_t compass_init();
uint8_t update_compass();

char mag_x_str[16];
char mag_y_str[16];
char mag_z_str[16];

int16_t mag_direction;

#endif