/*Inclusions*/
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "Adafruit_GPS_project.h"
#include "serial.h"

int main(void)
{
	char input; 
	char temp[100];
	char output_buf[256]; 

	char longitude_str[20];
	char latitude_str[20];
		
	int input_len;
	// Initialize the serial and gps interface
	serial_init();
	gps_common_init();

	//From here, we just want to continuously poll the gps for data and try to display that data
	int count = 0;
	while(1)
	{
		_delay_ms(1);
				
		/************* use the following block to print out raw data from the gps module */

		// input = serial_in(); 
		// serial_out(input);

		/*************************************************************** **********/


		/***** use the following block to print out gps data as a raw string */
		//For testing, we will send this string out to the console anyway to check we are getting the right data in the main loop
		
		//strcpy(output_buf, "Raw GPS Data: ");
		//strcpy(output_buf, input);
		//input_len = strlen(output_buf);
		//output_buf[input_len] = input;
		//output_buf[input_len + 1] = '\r';
		//output_buf[input_len + 2] = '\n';
		//output_buf[input_len + 3] = '\0';
		//snprintf(output_buf, sizeof(output_buf), "Raw GPS Data: %s\n", input);
		//serial_string_out(output_buf);
		//memset(output_buf, 0, sizeof output_buf);

		/**************************************************************************/
		
		
		input = gps_read();
		//strcpy(temp, gps_read());
		count++;
		//serial_string_out(temp);

		//Check if the data is new (Uncomment as testing proceeds)
		if(gps_newNMEAreceived() == true)
		{
			//Parse data stored in Adafruit_GPS code
			gps_parse(gps_lastNMEA()); 

			//We should now have date, time, and location data accessible here
			
			//input_len = strlen(output_buf);
			//output_buf[input_len + 1] = '\r';
			//output_buf[input_len + 1] = '\n';
			//output_buf[input_len + 2] = '\0';
			//serial_string_out(output_buf);
			//memset(output_buf, 0, sizeof output_buf);
		}

		if(count > 300){
			
			dtostrf(latitude, 5, 5, latitude_str);			
			dtostrf(longitude, 5, 5, longitude_str);

			snprintf(output_buf, sizeof(output_buf), "Date: Day: %u, Month: %u, Year: %u, \r\n"
							"Time: %u:%u:%u\r\nLocation: %s%c, %s%c, Fix: %d\r\n"
							", Fix quality: %u, Num Satellites: %u\r\n", day, month, year, hour, minute, 
							seconds, latitude_str, lat, longitude_str, lon, fix, fixquality, satellites);
			input_len = strlen(output_buf);
			serial_string_out(output_buf);
			memset(output_buf, 0, sizeof output_buf);
			count = 0;
		}

	}
}