/*
EE459 Spring 2019
Team 15
Project name: GeoBuddy
Ling Ye
Brian Suitt
Yi Sui
*/

/*
GeoBuddy.h and GeoBuddy.c contain the main program and other
variables used by the main program
*/

#ifndef GEOBUDDY_H
#define GEOBUDDY_H
	
void update_user_position();
void update_distance();

char serial_output_buf[256];
char lcd_output_buf[45];

double goal_lat;
double goal_long;
char goal_lat_str[15];
char goal_long_str[15];
char goal_name[30];
char goal_data[361];

double curr_lat;
double curr_long;
char curr_lat_str[15];
char curr_long_str[15];

int curr_distance;
char curr_distance_str[10];

double brng;
char brng_str[5];
int16_t brng_int;
uint16_t curr_direction;
char curr_direction_str[3];

uint16_t background_color;
uint16_t background_color_test;
uint16_t text_color;
uint16_t arrow_color;

uint16_t arrive_threshold = 50;

uint8_t state;

#endif
