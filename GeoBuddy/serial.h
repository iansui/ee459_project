/*
EE459 Spring 2019
Team 15
Project name: GeoBuddy
Ling Ye
Brian Suitt
Yi Sui
*/

/*
serial.c and serial.h initiazlie and handle the USART communication between
the microcontroller and the gps module.
they can also be used to send out data thru the serial port on the project
board for debugging
*/

#ifndef SERIAL_H
#define SERIAL_H

#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#define SERIAL_FOSC 7372800
#define SERIAL_BAUD 9600
#define the_ubrr (SERIAL_FOSC/16/SERIAL_BAUD-1)

void serial_init ();
void serial_out ( char ch );
void serial_string_out ( char* ch );
char serial_in ();

#endif