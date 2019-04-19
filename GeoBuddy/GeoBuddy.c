//sudo picocom --baud 9600 /dev/ttyUSB0

/*Inclusions*/
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

void location_init(){

    location_index = 0;
	location_size = 2;

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


void update_user_location(){

	//update current user location
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



void update_distance(){

	//don't update curr_distance if fix is 0
	if(fix == 0){
		return;
	}

	//current current distance between the use and the goal location
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

	//calculate current direction of the goal location
	double x = sin(goal_long_rad - curr_long_rad)* cos(goal_lat_rad);
	double y = cos(curr_lat_rad)*sin(goal_lat_rad) - sin(curr_lat_rad)*cos(goal_lat_rad)*cos(goal_long_rad-curr_long_rad);
	brng = (atan2(y, x) * 180 /M_PI);
	dtostrf(brng, 10, 7, brng_str);
	
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

	

/*
	//output current distance and direction data through serial connection
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

void drawGPS(){
	
	//if fix is 0, don't print any data yet
	if(fix == 0){
		draw_box(10, 10, LCD_Width-20, 20, background_color_test);
		strcpy(lcd_output_buf, "Fetching GPS DATA...");
		drawString(lcd_output_buf, 50, 12, 12, text_color, 1);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

		// snprintf(lcd_output_buf, sizeof(lcd_output_buf), "mag dir: %i", mag_direction);
		// draw_box(10, 30, LCD_Width-20, 40, background_color);
		// drawString(lcd_output_buf, 50, 12, 32, text_color, 1);
		// memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

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

		//show the distance between current and goal
		snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Distance: %s %s feet", curr_direction_str,  curr_distance_str);
		draw_box(10, 70, LCD_Width-20, 80, background_color_test);
		drawString(lcd_output_buf, 50, 12, 72, text_color, 1);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

		//draw direction arrow
		draw_box(70, 150, 170, 250, background_color_test);
		drawDirectionArrow(curr_direction, arrow_color);

		// for(int i = 0; i < 8; i++){
		// 	draw_box(70, 150, 170, 250, background_color);
		// 	drawDirectionArrow(i, arrow_color);
		// 	_delay_ms(100);
		// }


/*
		if(curr_distance > arrive_threshold){
			//draw direction arrow
			draw_box(70, 100, 170, 200, background_color);
			drawDirectionArrow(curr_direction, arrow_color);
		}
		else{
			draw_box(10, 100, 230, 200, background_color);
			drawParagragh(goal_data, sizeof(goal_data), text_color);
			// _delay_ms(1000);
			draw_box(10, 100, 230, 200, background_color_test);
		}

	*/
	}
}


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

	snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Est. Compass: %s", compass_heading);
		draw_box(10, 90, LCD_Width-20, 100, background_color_test);
		drawString(lcd_output_buf, 50, 12, 92, text_color, 1);
		memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

}

void drawTouch(){


	//draw test region
	if(touches > 0){
			draw_box(LCD_Width-100, 250, LCD_Width -1, LCD_Height-1, color565(255, 255, 0));
	}
	else{
			draw_box(LCD_Width-100, 250, LCD_Width -1, LCD_Height-1, color565(0, 255, 0));
	}

	snprintf(lcd_output_buf, sizeof(lcd_output_buf), "num of touches: %i", touches);
	draw_box(10, 250, LCD_Width-100, 260, background_color);
	drawString(lcd_output_buf, 50, 12, 252, text_color, 1);
	memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

	snprintf(lcd_output_buf, sizeof(lcd_output_buf), "point: %i %i", touchX[0], touchY[0]);
	draw_box(10, 270, LCD_Width-100, 280, background_color);
	drawString(lcd_output_buf, 50, 12, 272, text_color, 1);
	memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

}


void drawArrive(){

	//show the name of the goal location
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
	state = 1;

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

	//draw background
	draw_box(0, 0, LCD_Width-1, LCD_Height-1, background_color);

	// snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Goal:     %s, %s", goal_lat_str, goal_long_str);
	// 	drawString(lcd_output_buf, 50, 12, 252, text_color, 1);
	// 	memset(lcd_output_buf, 0, sizeof(lcd_output_buf));
	
	// draw_box(10, 10, LCD_Width-20, 20, background_color);
	// draw_box(10, 30, LCD_Width-20, 40, background_color);
	// draw_box(10, 50, LCD_Width-20, 60, background_color);
	// draw_box(10, 70, LCD_Width-20, 80, background_color);
	// draw_box(70, 100, 170, 200, background_color);


	state = 2;

	//start infinite loop
	while(1){

		_delay_ms(1);

		++gps_timer;

		//state 2
		if(state == 2 && gps_timer == 15){
			gps_read_new();
			gps_timer = 0;
			drawGPS();

			if(fix != 0){
				state = 3;
			}
		}

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

		if(state == 4){
			drawArrive();

			draw_box(10, 50, 230, 190, background_color_test);
			drawParagragh(goal_data, sizeof(goal_data), text_color);

			//draw arrive button
			draw_box(0, 220, LCD_Width-1, LCD_Height-1, text_color);
			snprintf(lcd_output_buf, sizeof(lcd_output_buf), "Next");
			drawString(lcd_output_buf, 50, 70, 255, background_color, 4);
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

		if(state == 5){
			draw_box(0, 0, LCD_Width-1, LCD_Height-1, background_color);
			strcpy_P(lcd_output_buf, (char *)pgm_read_word(&(location_name_table[location_index+1])));
			drawParagragh(lcd_output_buf, sizeof(lcd_output_buf), text_color);
			memset(lcd_output_buf, 0, sizeof(lcd_output_buf));

			while(1){

			}

		}

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

		// snprintf(serial_output_buf, sizeof(serial_output_buf),
		// "state: %i\r\n", state);
		// serial_string_out(serial_output_buf);
		// memset(serial_output_buf, 0, sizeof(serial_output_buf));

		// if(gps_timer == 15){
		// 	gps_read_new();

		// 	update_user_location();

		// 	update_distance();


		// 	update_compass();

			
		// 	drawGPS();
		// 	gps_timer = 0;

		// }

		// update_touch();
		// drawTouch();

	}

	
}