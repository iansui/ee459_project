#ifndef _GEOBUDDY_H
#define _GEOBUDDY_H
	
void update_user_position();
void distance(double goal_lat, double goal_long);
uint16_t curr_direction;
char curr_direction_str[3];


//goal is set to the rough location of the lab room
double goal_lat = 34.020506;
double goal_long = -118.289114;

//curr is the signed current location
double curr_lat;
double curr_long;

int curr_distance;
double brng;

uint16_t background_color;
uint16_t text_color;

#endif
