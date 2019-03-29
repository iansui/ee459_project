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

void update_user_position(){

	char longitude_str[20];
	char latitude_str[20];
	char output_buf[256];

	//****************  output parsed GPS data  ******/		
	dtostrf(latitudeDegrees, 10, 7, latitude_str);	
	dtostrf(longitudeDegrees, 10, 7, longitude_str);

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
					lat, latitude_str, lon, longitude_str,
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

	if(fix == 0){
		return;
	}

	long int R = 6371000;
	double curr_lat_rad = latitudeDegrees * M_PI / 180;
	double goal_lat_rad = goal_lat * M_PI / 180;
	double lat_diff = (goal_lat - latitudeDegrees) * M_PI / 180;
	double long_diff = (goal_long - longitudeDegrees) * M_PI / 180;
	double a = sin(lat_diff/2) * sin(lat_diff/2) + cos(curr_lat_rad) * cos(goal_lat_rad) * sin(long_diff/2) * sin(long_diff/2);
	double c = 2 * atan2(sqrt(a), sqrt(1-a));
	double distance = R * c;

	char distance_str[20];
	char goal_lat_str[20];
	char goal_long_str[20];
	char longitude_str[20];
	char latitude_str[20];
	dtostrf(latitudeDegrees, 10, 7, latitude_str);	
	dtostrf(longitudeDegrees, 10, 7, longitude_str);
	dtostrf(goal_lat, 10, 7, goal_lat_str);	
	dtostrf(goal_long, 10, 7, goal_long_str);
	char output_buf[256];
	dtostrf(distance, 12, 7, distance_str);			
	snprintf(output_buf, sizeof(output_buf),
	 "Distance: %s meters\r\n"
	 "Goal loc: %s, %s\r\n"
	 "Current: %s, %s\r\n"
	 , distance_str, goal_lat_str, goal_long_str, latitude_str, longitude_str);
	serial_string_out(output_buf);
	memset(output_buf, 0, sizeof(output_buf));
}

int main(void){
	
	// Initialize the serial and gps interface
	serial_init();
	gps_common_init();

	int gps_iteration_count = 0;

	//dummy is set to the rough location of the lab room
	double dummy_lat = 34.020520;
	double dummy_long = -118.289709;

	while(1){

		_delay_ms(1);
			
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
		}
		//end of user position update
	}
}