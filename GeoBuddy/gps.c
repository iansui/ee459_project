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

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <util/delay.h>

#include "gps.h"

// how long are max NMEA lines to parse?
#define MAXLINELENGTH 120

// we double buffer: read one line in and leave one for the main program
volatile char line1[MAXLINELENGTH];
volatile char line2[MAXLINELENGTH];
// our index into filling the current line
volatile uint8_t lineidx=0;
// pointers to the double buffers
volatile char *currentline;
volatile char *lastline;
volatile bool recvdflag;

bool gps_parse(char *nmea) {

  // first look if we even have one
  if (nmea[strlen(nmea)-4] == '*') {
    uint16_t sum = parseHex(nmea[strlen(nmea)-3]) * 16;
    sum += parseHex(nmea[strlen(nmea)-2]);
    
    uint8_t i=0;

    // check checksum 
    for (i=2; i < (strlen(nmea)-4); i++) {
      sum ^= nmea[i];
    }
    if (sum != 0) {
      // bad checksum :(
      return false;
    }
  }
  int32_t degree;
  long minutes;
  char degreebuff[10];
  // look for a few common sentences

  if (strstr(nmea, "$GPGGA")) {
    // found GGA
    char *p = nmea;
    // get time
    p = strchr(p, ',')+1;
    float timef = atof(p);
    uint32_t time = timef;
    hour = time / 10000;
    minute = (time % 10000) / 100;
    seconds = (time % 100);
    milliseconds = fmod(timef, 1.0) * 1000;

    // parse out latitude
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      strncpy(degreebuff, p, 2);
      p += 2;
      degreebuff[2] = '\0';
      degree = atol(degreebuff) * 10000000;
      strncpy(degreebuff, p, 2); // minutes
      p += 3; // skip decimal point
      strncpy(degreebuff + 2, p, 4);
      degreebuff[6] = '\0';
      minutes = (50 * atol(degreebuff) / 3);
      // latitude_fixed = (degree + minutes);

      lat_comp = (degree / 10000000)+(minutes * 0.000006F / 60);

      // latitude = (degree / 100000 + minutes * 0.000006F);
      // latitude_decimal = (degree / 10000000) + minutes * 0.0000001F;
      // latitudeDegrees = ((latitude-100*(int)(latitude/100))/60.0);
      // latitudeDegrees += (int)(latitude/100);
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      // if (p[0] == 'S') latitudeDegrees *= -1.0;
      if (p[0] == 'N') lat = 'N';
      else if (p[0] == 'S') lat = 'S';
      else if (p[0] == ',') lat = 0;
      else return false;
    }
    
    // parse out longitude
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      strncpy(degreebuff, p, 3);
      p += 3;
      degreebuff[3] = '\0';
      degree = atol(degreebuff) * 10000000;
      strncpy(degreebuff, p, 2); // minutes
      p += 3; // skip decimal point
      strncpy(degreebuff + 2, p, 4);
      degreebuff[6] = '\0';
      minutes = 50 * atol(degreebuff) / 3;
      // longitude_fixed = degree + minutes;

      long_comp = (degree / 10000000)+(minutes * 0.000006F / 60);

      // longitude = degree / 100000 + minutes * 0.000006F;
      // longitude_decimal = (degree / 10000000) + minutes * 0.0000001F;
      // longitudeDegrees = (longitude-100*(int)(longitude/100))/60.0;
      // longitudeDegrees += (int)(longitude/100);
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      // if (p[0] == 'W') longitudeDegrees *= -1.0;
      if (p[0] == 'W') lon = 'W';
      else if (p[0] == 'E') lon = 'E';
      else if (p[0] == ',') lon = 0;
      else return false;
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      fixquality = atoi(p);
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      satellites = atoi(p);
    }
    
    p = strchr(p, ',')+1;
    // if (',' != *p)
    // {
    //   HDOP = atof(p);
    // }
    
    p = strchr(p, ',')+1;
    // if (',' != *p)
    // {
    //   altitude = atof(p);
    // }
    
    p = strchr(p, ',')+1;
    p = strchr(p, ',')+1;
    // if (',' != *p)
    // {
    //   geoidheight = atof(p);
    // }
    return true;
  }
  if (strstr(nmea, "$GPRMC")) {
   // found RMC
    char *p = nmea;

    // get time
    p = strchr(p, ',')+1;
    float timef = atof(p);
    uint32_t time = timef;
    hour = time / 10000;
    minute = (time % 10000) / 100;
    seconds = (time % 100);
    milliseconds = fmod(timef, 1.0) * 1000;

    p = strchr(p, ',')+1;
    // Serial.println(p);
    if (p[0] == 'A') 
      fix = true;
    else if (p[0] == 'V')
      fix = false;
    else
      return false;

    // parse out latitude
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      strncpy(degreebuff, p, 2);
      p += 2;
      degreebuff[2] = '\0';
      long degree = atol(degreebuff) * 10000000;
      strncpy(degreebuff, p, 2); // minutes
      p += 3; // skip decimal point
      strncpy(degreebuff + 2, p, 4);
      degreebuff[6] = '\0';
      long minutes = 50 * atol(degreebuff) / 3;
      // latitude_fixed = degree + minutes;

      lat_comp = (degree / 10000000)+(minutes * 0.000006F / 60);

      // latitude = degree / 100000 + minutes * 0.000006F;
      // latitude_decimal = (degree / 10000000) + minutes * 0.0000001F;
      // latitudeDegrees = (latitude-100*(int)(latitude/100))/60.0;
      // latitudeDegrees += (int)(latitude/100);
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      // if (p[0] == 'S') latitudeDegrees *= -1.0;
      if (p[0] == 'N') lat = 'N';
      else if (p[0] == 'S') lat = 'S';
      else if (p[0] == ',') lat = 0;
      else return false;
    }
    
    // parse out longitude
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      strncpy(degreebuff, p, 3);
      p += 3;
      degreebuff[3] = '\0';
      degree = atol(degreebuff) * 10000000;
      strncpy(degreebuff, p, 2); // minutes
      p += 3; // skip decimal point
      strncpy(degreebuff + 2, p, 4);
      degreebuff[6] = '\0';
      minutes = 50 * atol(degreebuff) / 3;
      // longitude_fixed = degree + minutes;

      long_comp = (degree / 10000000)+(minutes * 0.000006F / 60);

      // longitude = degree / 100000 + minutes * 0.000006F;
      // longitude_decimal = (degree / 10000000) + minutes * 0.0000001F;
      // longitudeDegrees = (longitude-100*(int)(longitude/100))/60.0;
      // longitudeDegrees += (int)(longitude/100);
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      // if (p[0] == 'W') longitudeDegrees *= -1.0;
      if (p[0] == 'W') lon = 'W';
      else if (p[0] == 'E') lon = 'E';
      else if (p[0] == ',') lon = 0;
      else return false;
    }
    // speed
    p = strchr(p, ',')+1;
    // if (',' != *p)
    // {
    //   speed = atof(p);
    // }
    
    // angle
    p = strchr(p, ',')+1;
    // if (',' != *p)
    // {
    //   angle = atof(p);
    // }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
      uint32_t fulldate = atof(p);
      day = fulldate / 10000;
      month = (fulldate % 10000) / 100;
      year = (fulldate % 100);
    }
    // we dont parse the remaining, yet!
    return true;
  }

  return false;
}

char gps_read(char c) {

  // char c = 0;
  
  // c = serial_in();

  if (c == '\n') {
    currentline[lineidx] = 0;

    if (currentline == line1) {
      currentline = line2;
      lastline = line1;
    } else {
      currentline = line1;
      lastline = line2;
    }

    lineidx = 0;
    recvdflag = true;
  }

  currentline[lineidx++] = c;
  if (lineidx >= MAXLINELENGTH)
    lineidx = MAXLINELENGTH-1;

  return c;
}

void gps_read_new(void){
  char c;

  while(true){
      c = serial_in();

      if(c == '$'){

        gps_read(c);

        while(true){
          c = serial_in();
          gps_read(c);
          if(c == '\n'){
            break;
          }
        }

        if(gps_newNMEAreceived() == true){
          bool parse_result = gps_parse(gps_lastNMEA()); 
          if(!parse_result){
        	  continue;
          }
          else{
            break;
          }

        }
      }
  }
}

// Initialization code 
void gps_common_init(void) {
  recvdflag   = false;
  paused      = false;
  lineidx     = 0;
  currentline = line1;
  lastline    = line2;

  hour = minute = seconds = year = month = day =
    fixquality = satellites = 0; // uint8_t
  lat = lon = 0;
  // = mag = 0; // char
  fix = false; // boolean
  milliseconds = 0; // uint16_t
  //latitude = longitude = 
  // geoidheight = altitude =
  // speed = angle = magvariation = HDOP = 0.0; // float
}

bool gps_newNMEAreceived(void) {
  return recvdflag;
}

/*
void gps_pause(bool p) {
  paused = p;
}
*/

char *gps_lastNMEA(void) {
  recvdflag = false;
  return (char *)lastline;
}

// read a Hex value and return the decimal equivalent
uint8_t parseHex(char c) {
    if (c < '0')
      return 0;
    if (c <= '9')
      return c - '0';
    if (c < 'A')
       return 0;
    if (c <= 'F')
       return (c - 'A')+10;
    // if (c > 'F')
    return 0;
}