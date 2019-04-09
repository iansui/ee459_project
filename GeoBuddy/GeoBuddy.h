#ifndef GEOBUDDY_H
#define GEOBUDDY_H
	
void update_user_position();
void update_distance(double goal_lat, double goal_long);

char serial_output_buf[256];
char lcd_output_buf[50];

double goal_lat = 34.020506;
double goal_long = -118.289114;
char goal_lat_str[20];
char goal_long_str[20];

double curr_lat;
double curr_long;
char curr_lat_str[20];
char curr_long_str[20];

int curr_distance;
char curr_distance_str[20];

double brng;
char brng_str[20];
uint16_t curr_direction;
char curr_direction_str[3];

uint16_t background_color;
uint16_t background_color1;
uint16_t text_color;

#endif
