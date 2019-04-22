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