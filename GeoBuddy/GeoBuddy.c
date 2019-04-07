/*Inclusions*/
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "Adafruit_GPS_project.h"
#include "serial.h"
#include "string.h"
#include <stdlib.h>
#include <math.h>
#include "GeoBuddy.h"
#include "draw.h"

//sudo picocom --baud 9600 /dev/ttyUSB0

void update_user_position(){

	char longitude_str[20];
	char latitude_str[20];
	char output_buf[256];

	//****************  output parsed GPS data  ******/	
	lat_comp_signed = lat_comp;	
	long_comp_signed = long_comp;	
	if(lat == 'S'){
        lat_comp_signed *= -1;
    }
	if(lon == 'W'){
        long_comp_signed *= -1;
    }

	dtostrf(lat_comp_signed, 10, 7, latitude_str);
	dtostrf(long_comp_signed, 10, 7, longitude_str);

	if(fix == 0){

		snprintf(output_buf, sizeof(output_buf), "DateTime: %u-%u-%u %u:%u:%u \r\n"
					"Fix: %d, Fix quality: %u, Num Satellites: %u\r\n",
					year, month, day, hour, minute, seconds, 
					fix, fixquality, satellites);
	}
	else{
		snprintf(output_buf, sizeof(output_buf), "DateTime: %u-%u-%u %u:%u:%u \r\n"
					"Location: %c %s, %c %s\r\n"
					"Fix: %d, Fix quality: %u, Num Satellites: %u\r\n",
					year, month, day, hour, minute, seconds, 
					lat, latitude_str,  lon, longitude_str,
					fix, fixquality, satellites);

	}

	serial_string_out(output_buf);
	memset(output_buf, 0, sizeof(output_buf));
}


void distance(double goal_lat, double goal_long){

	// var R = 6371e3; // meters
	// var φ1 = lat1.toRadians();
	// var φ2 = lat2.toRadians();
	// var Δφ = (lat2-lat1).toRadians();
	// var Δλ = (lon2-lon1).toRadians();
	// var a = Math.sin(Δφ/2) * Math.sin(Δφ/2) +
	// 		Math.cos(φ1) * Math.cos(φ2) *
	// 		Math.sin(Δλ/2) * Math.sin(Δλ/2);
	// var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
	// var d = R * c;


	// var y = Math.sin(λ2-λ1) * Math.cos(φ2);
	// var x = Math.cos(φ1)*Math.sin(φ2) -
	// Math.sin(φ1)*Math.cos(φ2)*Math.cos(λ2-λ1);
	// var brng = Math.atan2(y, x).toDegrees();

	if(fix == 0){
		return;
	}

	long int R = 6371000;
	double curr_lat_rad = lat_comp_signed * M_PI / 180;
	double curr_long_rad = long_comp_signed * M_PI / 180;
	double goal_lat_rad = goal_lat * M_PI / 180;
	double goal_long_rad = goal_long * M_PI / 180;
	double lat_diff = (goal_lat - lat_comp_signed) * M_PI / 180;
	double long_diff = (goal_long - long_comp_signed) * M_PI / 180;
	double a = sin(lat_diff/2) * sin(lat_diff/2) + cos(curr_lat_rad) * cos(goal_lat_rad) * sin(long_diff/2) * sin(long_diff/2);
	double c = 2 * atan2(sqrt(a), sqrt(1-a));
	double distance = R * c * 3.28084;

	double x = sin(goal_long_rad - curr_long_rad)* cos(goal_lat_rad);
	double y = cos(curr_lat_rad)*sin(goal_lat_rad) - sin(curr_lat_rad)*cos(goal_lat_rad)*cos(goal_long_rad-curr_long_rad);
	double brng = (atan2(y, x) * 180 /M_PI);

	char goal_direction_str[3];

	if(brng > -22.5 && brng <= 22.5){
		goal_direction = 0;
		strcpy(goal_direction_str, " E");
	}
	else if(brng > 22.5 && brng <= 67.5){
		goal_direction = 1;
		strcpy(goal_direction_str, "NE");
	}
	else if(brng > 67.5 && brng <= 112.5){
		goal_direction = 2;
		strcpy(goal_direction_str, " N");
	}
	else if(brng > 112.5  && brng <= 157.5){
		goal_direction = 3;
		strcpy(goal_direction_str, "NW");
	}
	else if(brng > 157.5 || brng <= -157.5){
		goal_direction = 4;
		strcpy(goal_direction_str, " W");
	}
	else if(brng > -157.5 && brng <= -112.5){
		goal_direction = 5;
		strcpy(goal_direction_str, "SW");
	}
	else if(brng > -112.5 && brng <= -67.5){
		goal_direction = 6;
		strcpy(goal_direction_str, " S");
	}
	else {
		goal_direction = 7;
		strcpy(goal_direction_str, "SE");
	}

	char distance_str[20];
	char goal_lat_str[20];
	char goal_long_str[20];
	char longitude_str[20];
	char latitude_str[20];
	char brng_str[20];
	dtostrf(lat_comp_signed, 10, 7, latitude_str);	
	dtostrf(long_comp_signed, 10, 7, longitude_str);
	dtostrf(goal_lat, 10, 7, goal_lat_str);	
	dtostrf(goal_long, 10, 7, goal_long_str);
	dtostrf(brng, 10, 7, brng_str);
	char output_buf[256];
	dtostrf(distance, 12, 7, distance_str);			
	snprintf(output_buf, sizeof(output_buf),
	 "Goal loc: %s, %s\r\n"
	 "Current: %s, %s\r\n"
	 "Direction: %s %s\r\n"
	 "Distance: %s feet\r\n"
	 , goal_lat_str, goal_long_str, latitude_str, longitude_str, goal_direction_str, brng_str, distance_str);
	serial_string_out(output_buf);
	memset(output_buf, 0, sizeof(output_buf));
}

void drawGPS(){
	char output_buf[40];
	if(fix == 0){
		strcpy(output_buf, "Fix is 0");
		drawString(output_buf, 8, 10, 10, color565(255, 255, 255));
	}
	else{
		char longitude_str[20];
	char latitude_str[20];
	dtostrf(lat_comp_signed, 10, 7, latitude_str);	
	dtostrf(long_comp_signed, 10, 7, longitude_str);
		snprintf(output_buf, sizeof(output_buf), "Location: %s, %s", latitude_str, longitude_str);

		//clear old info
		draw_box(10, 10, LCD_Width-1, 20, color565(255,255,255));

		drawString(output_buf, 40, 10, 10, color565(0, 0, 0));
	}
}

int main(void){

	//initialize the LCD
	DDRB |= LCD_Data_B;         // Set PORTB bits 0-1 for output
    DDRD |= LCD_Data_D;         // Set PORTD bits 2-7 for output
    PORTC |= LCD_Ctrl_B;        // Set all the control lines high
    DDRC |= LCD_Ctrl_B;         // Set control port bits for output
    initialize();               // Initialize the LCD display
	
	// Initialize the serial and gps interface
	serial_init();
	gps_common_init();

	int gps_iteration_count = 0;

	//dummy is set to the rough location of the lab room
	double dummy_lat = 34.020506;
	double dummy_long = -118.289114;

	//draw white background
	draw_box(0, 0, LCD_Width-1, LCD_Height-1, color565(255,255,255));


	while(1){

		_delay_ms(1);
		//drawGPS();
		//_delay_ms(300);
			
		//update user position
		gps_iteration_count++;
		gps_read();

		//only parse and update if the data is new
		if(gps_newNMEAreceived() == true){
			gps_parse(gps_lastNMEA()); 
		}
		if(gps_iteration_count > 300){
			update_user_position(); 
			distance(dummy_lat, dummy_long);
			gps_iteration_count = 0;
			
			drawGPS();
			_delay_ms(100);
		}
		//end of user position update

		
	}
}