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

//  use the following line to get serial output
//  sudo picocom --baud 9600 /dev/ttyUSB0

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <util/delay.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "gps.h"
#include "serial.h"
#include "lcd.h"
#include "GeoBuddy.h"
#include "i2c.h"
#include "compass.h"
#include "location.h"


//	initialize location data
void location_init(){

    location_index = 0;
	location_size = 4;

	memset(goal_name, 0, sizeof(goal_name));
	strcpy_P(goal_name, (char *)pgm_read_word(&(location_name_table[location_index])));
	
	memset(goal_data, 0, sizeof(goal_data));
	strcpy_P(goal_data, (char *)pgm_read_word(&(location_data_table[location_index])));

	memset(goal_lat_str, 0, sizeof(goal_lat_str));
	strcpy_P(goal_lat_str, (char *)pgm_read_word(&(location_lat_table[location_index])));

	memset(goal_long_str, 0, sizeof(goal_long_str));
	strcpy_P(goal_long_str, (char *)pgm_read_word(&(location_long_table[location_index])));

	goal_lat = atof(goal_lat_str);
	goal_long = atof(goal_long_str);
}

//	load next location
void location_load_next(){

	location_index++;

	memset(goal_name, 0, sizeof(goal_name));
	strcpy_P(goal_name, (char *)pgm_read_word(&(location_name_table[location_index])));
	
	memset(goal_data, 0, sizeof(goal_data));
	strcpy_P(goal_data, (char *)pgm_read_word(&(location_data_table[location_index])));

	memset(goal_lat_str, 0, sizeof(goal_lat_str));
	strcpy_P(goal_lat_str, (char *)pgm_read_word(&(location_lat_table[location_index])));

	memset(goal_long_str, 0, sizeof(goal_long_str));
	strcpy_P(goal_long_str, (char *)pgm_read_word(&(location_long_table[location_index])));

	goal_lat = atof(goal_lat_str);
	goal_long = atof(goal_long_str);
}


//	update user current location
void update_user_location(){

	curr_lat = lat_comp;	
	curr_long = long_comp;	
	if(lat == 'S'){
        curr_lat *= -1;
    }
	if(lon == 'W'){
        curr_long *= -1;
    }

	dtostrf(curr_lat, 10, 7, curr_lat_str);
	dtostrf(curr_long, 10, 7, curr_long_str);

	// output data to serial
/*
	if(fix == 0){
		snprintf(serial_output_buf, sizeof(serial_output_buf), "DateTime: %u-%u-%u %u:%u:%u \r\n"
					"Fix: %d, Fix quality: %u, Num Satellites: %u\r\n",
					year, month, day, hour, minute, seconds, 
					fix, fixquality, satellites);
	}
	else{
		snprintf(serial_output_buf, sizeof(serial_output_buf), "DateTime: %u-%u-%u %u:%u:%u \r\n"
					"Location: %c %s, %c %s\r\n"
					"Fix: %d, Fix quality: %u, Num Satellites: %u\r\n",
					year, month, day, hour, minute, seconds, 
					lat, curr_lat_str, lon, curr_long_str,
					fix, fixquality, satellites);
	}
	serial_string_out(serial_output_buf);
	memset(serial_output_buf, 0, sizeof(serial_output_buf));

	*/
}

// calculate distance and brng between curr and goal locations
void update_distance(){

	//don't update curr_distance if fix is 0
	if(fix == 0){
		return;
	}

	long int R = 6371000;
	double curr_lat_rad = curr_lat * M_PI / 180;
	double curr_long_rad = curr_long * M_PI / 180;
	double goal_lat_rad = goal_lat * M_PI / 180;
	double goal_long_rad = goal_long * M_PI / 180;
	double lat_diff = (goal_lat - curr_lat) * M_PI / 180;
	double long_diff = (goal_long - curr_long) * M_PI / 180;
	double a = sin(lat_diff/2) * sin(lat_diff/2) + cos(curr_lat_rad) * cos(goal_lat_rad) * sin(long_diff/2) * sin(long_diff/2);
	double c = 2 * atan2(sqrt(a), sqrt(1-a));
	curr_distance = (int)(R * c * 3.28084);
	dtostrf(curr_distance, 1, 0, curr_distance_str);			

	double x = sin(goal_long_rad - curr_long_rad)* cos(goal_lat_rad);
	double y = cos(curr_lat_rad)*sin(goal_lat_rad) - sin(curr_lat_rad)*cos(goal_lat_rad)*cos(goal_long_rad-curr_long_rad);
	brng = (atan2(y, x) * 180 /M_PI);
	brng_int = (int16_t)brng;
	
	if(brng > -22.5 && brng <= 22.5){
		curr_direction = 0;
		strcpy(curr_direction_str, "E ");
	}
	else if(brng > 22.5 && brng <= 67.5){
		curr_direction = 1;
		strcpy(curr_direction_str, "NE");
	}
	else if(brng > 67.5 && brng <= 112.5){
		curr_direction = 2;
		strcpy(curr_direction_str, "N ");
	}
	else if(brng > 112.5  && brng <= 157.5){
		curr_direction = 3;
		strcpy(curr_direction_str, "NW");
	}
	else if(brng > 157.5 || brng <= -157.5){
		curr_direction = 4;
		strcpy(curr_direction_str, "W ");
	}
	else if(brng > -157.5 && brng <= -112.5){
		curr_direction = 5;
		strcpy(curr_direction_str, "SW");
	}
	else if(brng > -112.5 && brng <= -67.5){
		curr_direction = 6;
		strcpy(curr_direction_str, "S ");
	}
	else {
		curr_direction = 7;
		strcpy(curr_direction_str, "SE");
	}

	// output data to serial
/*
	snprintf(serial_output_buf, sizeof(serial_output_buf),
	 "Goal loc: %s, %s\r\n"
	 "Current: %s, %s\r\n"
	 "Direction: %s %s\r\n"
	 "Distance: %s feet\r\n"
	 , goal_lat_str, goal_long_str, curr_lat_str, curr_long_str, curr_direction_str, brng_str, curr_distance_str);
	serial_string_out(serial_output_buf);
	memset(serial_output_buf, 0, sizeof(serial_output_buf));
*/

}

//	draw user location data on the LCD
void drawGPS(){
	
	//if fix is 0, don't print any data yet
	if(fix == 0){
		draw_box(10, 10, LCD_Width-20, 20, background_color_test);
		strcpy(lcd_output_buf, "Fetching GPS DATA...");
		drawString(lcd_output_buf, 50, 12, 12, text_color, 1);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

		draw_box(10, 30, LCD_Width-20, 40, background_color_test);
		strcpy(lcd_output_buf, "GPS not ready");
		drawString(lcd_output_buf, 50, 12, 32, text_color, 1);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

		draw_box(10, 50, LCD_Width-20, 60, background_color_test);
		snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Fix: %d, Fix quality: %u", fix, fixquality);
		drawString(lcd_output_buf, 50, 12, 52, text_color, 1);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

		draw_box(10, 70, LCD_Width-20, 80, background_color_test);
		snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Num of Satellites: %u", satellites);
		drawString(lcd_output_buf, 50, 12, 72, text_color, 1);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));
	}
	else{
		//show the name of the goal location
		snprintf(lcd_output_buf, sizeof(lcd_output_buf), "%s", goal_name);
		draw_box(10, 10, LCD_Width-20, 20, background_color_test);
		drawString(lcd_output_buf, 50, 12, 12, text_color, 1);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

		//show the coordinates of the goal location
		snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Goal:     %s, %s", goal_lat_str, goal_long_str);
		draw_box(10, 30, LCD_Width-20, 40, background_color_test);
		drawString(lcd_output_buf, 50, 12, 32, text_color, 1);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

		//show the coordinates of the current location
		snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Current:  %s, %s", curr_lat_str, curr_long_str);
		draw_box(10, 50, LCD_Width-20, 60, background_color_test);
		drawString(lcd_output_buf, 50, 12, 52, text_color, 1);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

		//show the distance and brngbetween current and goal
		snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Distance: %i %s %s feet", brng_int, curr_direction_str,  curr_distance_str);
		draw_box(10, 70, LCD_Width-20, 80, background_color_test);
		drawString(lcd_output_buf, 50, 12, 72, text_color, 1);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

		//draw direction arrow
		draw_box(70, 150, 170, 250, background_color_test);
		drawDirectionArrow(curr_direction, arrow_color);
	}
}

//	draw compass data on the LCD
void drawCompass(){

	char compass_heading[3];

	if((mag_direction > 337.5 && mag_direction <= 360)|| (mag_direction > 0 && mag_direction <= 22.5)){
		strcpy(compass_heading, "E ");
	}
	else if(mag_direction > 22.5 && mag_direction <= 67.5){
		strcpy(compass_heading, "NE");
	}
	else if(mag_direction > 67.5 && mag_direction <= 112.5){
		strcpy(compass_heading, "N ");
	}
	else if(mag_direction > 112.5  && mag_direction <= 157.5){
		strcpy(compass_heading, "NW");
	}
	else if(mag_direction > 157.5 || mag_direction <= 202.5){
		strcpy(compass_heading, "W ");
	}
	else if(mag_direction > 202.5 && mag_direction <= 247.5){
		strcpy(compass_heading, "SW");
	}
	else if(mag_direction > 247.5 && mag_direction <= 292.5){
		strcpy(compass_heading, "S ");
	}
	else {
		strcpy(compass_heading, "SE");
	}

	snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Est. Compass Heading: %i %s", mag_direction, compass_heading);
		draw_box(10, 90, LCD_Width-20, 100, background_color_test);
		drawString(lcd_output_buf, 50, 12, 92, text_color, 1);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));
}

//	draw location name when user arrives
void drawArrive(){

	snprintf(lcd_output_buf, sizeof(lcd_output_buf), "%s", goal_name);
	draw_box(10, 10, LCD_Width-20, 20, background_color_test);
	drawString(lcd_output_buf, 50, 12, 12, text_color, 1);
	memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

	draw_box(10, 30, LCD_Width-20, 40, background_color_test);
	draw_box(10, 50, LCD_Width-20, 60, background_color_test);
	draw_box(10, 70, LCD_Width-20, 80, background_color_test);
	draw_box(10, 90, LCD_Width-20, 100, background_color_test);
	draw_box(70, 150, 170, 250, background_color_test);
}


int main(void){

	// init state
	state = 0;

	// initialize LCD
	DDRB |= LCD_Data_B;         // Set PORTB bits 0-1 for output
    DDRD |= LCD_Data_D;         // Set PORTD bits 2-7 for output
    PORTC |= LCD_Ctrl_B;        // Set all the control lines high
    DDRC |= LCD_Ctrl_B;         // Set control port bits for output
    lcd_init();               // Initialize the LCD display
	background_color = color565(220, 30, 58); // cardinal red
	background_color_test = color565(220, 30, 58);
	text_color = color565(255, 255, 0);
	arrow_color = color565(255, 255, 0);

	// initialize serial connection
	serial_init();

	// initialize gps
	gps_common_init();
	int gps_timer = 0;

	// initialize i2c
	i2c_init(BDIV);

	// initialize touch
	touch_init();	

	// initialize compass
	compass_init();

	// initialize location
	location_init();

	// draw background
	draw_box(0, 0, LCD_Width-1, LCD_Height-1, background_color);

	state = 1;

	//start infinite loop
	while(1){

		_delay_ms(1);

		++gps_timer;

		//  state 1, starting page
		if(state == 1){

			snprintf(lcd_output_buf, sizeof(lcd_output_buf), "GeoBuddy");
			drawString(lcd_output_buf, 50, 12, 30, text_color, 4);
			memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

			snprintf(lcd_output_buf, sizeof(lcd_output_buf), "EE459 Project");
			drawString(lcd_output_buf, 50, 12, 70, text_color, 2);
			memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

			snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Team 15");
			drawString(lcd_output_buf, 50, 12, 95, text_color, 2);
			memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

			// draw start button
			draw_box(0, 220, LCD_Width-1, LCD_Height-1, text_color);
			snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Start");
			drawString(lcd_output_buf, 50, 60, 255, background_color, 4);
			memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

			// get uset input
			while(true){
				update_touch();

				if(touches > 0 && touchX[0] >= 0 && touchY[0] >= 220){
					state = 2;
					draw_box(0, 0, LCD_Width-1, LCD_Height-1, background_color);
					break;
				}
			}
		}

		//  state 2, waiting for fix
		if(state == 2 && gps_timer == 15){
			gps_read_new();
			gps_timer = 0;
			drawGPS();

			if(fix != 0){
				state = 3;
			}
		}

		//  state 3, updating gps data
		if(state == 3){
			if(gps_timer == 15){
				gps_read_new();
				gps_timer = 0;

				if(fix == 0){
					state = 2;
					continue;
				}

				update_user_location();
				update_distance();
				drawGPS();

				update_compass();
				drawCompass();

				if(curr_distance <= arrive_threshold){
					state = 4;
				}

			}
		}

		// state 4, show location data
		if(state == 4){
			drawArrive();

			draw_box(10, 50, 230, 190, background_color_test);
			drawParagragh(goal_data, sizeof(goal_data), text_color);

			//draw button
			draw_box(0, 220, LCD_Width-1, LCD_Height-1, text_color);
			if(location_index == location_size-1){
				snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Finish");
				drawString(lcd_output_buf, 50, 50, 255, background_color, 4);
			}
			else{
				snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Next");
				drawString(lcd_output_buf, 50, 70, 255, background_color, 4);
			}
			memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

			while(true){
				update_touch();

				if(touches > 0 && touchX[0] >= 0 && touchY[0] >= 220){
					if(location_index == location_size-1){
						state = 5;
						break;
					}
					else{
						state = 6;
						break;
					}
				}
			}
		}

		// state 5, ending page
		if(state == 5){
			draw_box(0, 0, LCD_Width-1, LCD_Height-1, background_color);
			strcpy_P(lcd_output_buf, (char *)pgm_read_word(&(location_name_table[location_index+1])));
			drawParagragh(lcd_output_buf, sizeof(lcd_output_buf), text_color);
			memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

			//draw start button
			draw_box(0, 220, LCD_Width-1, LCD_Height-1, text_color);
			snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Back");
			drawString(lcd_output_buf, 50, 70, 255, background_color, 4);
			memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

			while(true){
				update_touch();

				if(touches > 0 && touchX[0] >= 0 && touchY[0] >= 220){
					state = 1;
					location_init();
					draw_box(0, 0, LCD_Width-1, LCD_Height-1, background_color);
					break;
				}
			}
		}

		// state 6, loading next location
		if(state == 6){
			draw_box(0, 0, LCD_Width-1, LCD_Height-1, background_color);
			location_load_next();
			gps_read_new();
			if(fix == 0){
				state = 2;
			}
			else{
				state = 3;
			}
		}
	}
}