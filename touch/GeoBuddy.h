#ifndef GEOBUDDY_H
#define GEOBUDDY_H
	
void update_user_position();
void update_distance(double goal_lat, double goal_long);

char serial_output_buf[256];
char lcd_output_buf[45];

double goal_lat;
double goal_long;
char goal_lat_str[15];
char goal_long_str[15];
char goal_title[30];
char goal_info[216];

double curr_lat;
double curr_long;
char curr_lat_str[15];
char curr_long_str[15];

int curr_distance;
char curr_distance_str[10];

double brng;
char brng_str[15];
uint16_t curr_direction;
char curr_direction_str[3];

uint16_t background_color;
uint16_t background_color_test;
uint16_t text_color;
uint16_t arrow_color;

int arrive_threshold = 50;

#endif
