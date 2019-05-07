/*
EE459 Spring 2019
Team 15
Project name: GeoBuddy
Ling Ye
Brian Suitt
Yi Sui
*/

/*
gps.h and gps.c handle the communication between the
microcontroller and the gps module thru USART
*/

#ifndef GPS_H
#define GPS_H
#include "serial.h"

uint8_t hour, minute, seconds, year, month, day;
uint16_t milliseconds;

float lat_comp, long_comp;
char lat, lon;
bool fix;
uint8_t fixquality, satellites;

bool paused;

void gps_begin(uint32_t baud); 
char *gps_lastNMEA(void);
bool gps_newNMEAreceived();
void gps_common_init(void);
bool parseNMEA(char *response);
uint8_t parseHex(char c);
char gps_read(char c);
void gps_read_new(void);
bool gps_parse(char *);

#endif