//#ifndef _ADAFRUIT_GPS_project_H
//#define _ADAFRUIT_GPS_project_H

//comment this out if you don't want to include software serial in the library
//#define USE_SW_SERIAL

// different commands to set the update rate from once a second (1 Hz) to 10 times a second (10Hz)
// Note that these only control the rate at which the position is echoed, to actually speed up the
// position fix you must also send one of the position fix rate commands below too.
#define PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ  "$PMTK220,10000*2F" // Once every 10 seconds, 100 millihertz.
#define PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ  "$PMTK220,5000*1B"  // Once every 5 seconds, 200 millihertz.
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_2HZ  "$PMTK220,500*2B"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"
// Position fix update rate commands.
#define PMTK_API_SET_FIX_CTL_100_MILLIHERTZ  "$PMTK300,10000,0,0,0,0*2C" // Once every 10 seconds, 100 millihertz.
#define PMTK_API_SET_FIX_CTL_200_MILLIHERTZ  "$PMTK300,5000,0,0,0,0*18"  // Once every 5 seconds, 200 millihertz.
#define PMTK_API_SET_FIX_CTL_1HZ  "$PMTK300,1000,0,0,0,0*1C"
#define PMTK_API_SET_FIX_CTL_5HZ  "$PMTK300,200,0,0,0,0*2F"
// Can't fix position faster than 5 times a second!


#define PMTK_SET_BAUD_57600 "$PMTK251,57600*2C"
#define PMTK_SET_BAUD_9600 "$PMTK251,9600*17"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

// to generate your own sentences, check out the MTK command datasheet and use a checksum calculator
// such as the awesome http://www.hhhh.org/wiml/proj/nmeaxor.html

#define PMTK_LOCUS_STARTLOG  "$PMTK185,0*22"
#define PMTK_LOCUS_STOPLOG "$PMTK185,1*23"
#define PMTK_LOCUS_STARTSTOPACK "$PMTK001,185,3*3C"
#define PMTK_LOCUS_QUERY_STATUS "$PMTK183*38"
#define PMTK_LOCUS_ERASE_FLASH "$PMTK184,1*22"
#define LOCUS_OVERLAP 0
#define LOCUS_FULLSTOP 1

#define PMTK_ENABLE_SBAS "$PMTK313,1*2E"
#define PMTK_ENABLE_WAAS "$PMTK301,2*2E"

// standby command & boot successful message
#define PMTK_STANDBY "$PMTK161,0*28"
#define PMTK_STANDBY_SUCCESS "$PMTK001,161,3*36"  // Not needed currently
#define PMTK_AWAKE "$PMTK010,002*2D"

// ask for the release and version
#define PMTK_Q_RELEASE "$PMTK605*31"

// request for updates on antenna status 
#define PGCMD_ANTENNA "$PGCMD,33,1*6C" 
#define PGCMD_NOANTENNA "$PGCMD,33,0*6D" 

// how long to wait when we're looking for a response
#define MAXWAITSENTENCE 10

/*
#if ARDUINO >= 100
 #include "Arduino.h"
#if defined (__AVR__) && !defined(__AVR_ATmega32U4__)
 #include "SoftwareSerial.h"
#endif
#else
 #include "WProgram.h"
 #include "NewSoftSerial.h"
#endif
*/



void gps_begin(uint32_t baud); 
char *gps_lastNMEA(void);
bool gps_newNMEAreceived();
void gps_common_init(void);

void gps_sendCommand(const char *);
  
void gps_pause(bool b);

bool parseNMEA(char *response);
uint8_t parseHex(char c);

char gps_read(void);
bool gps_parse(char *);

bool gps_wakeup(void);
bool gps_standby(void);

uint8_t hour, minute, seconds, year, month, day;
uint16_t milliseconds;
  // Floating point latitude and longitude value in degrees.
float latitude, longitude;

float lat_comp, long_comp;

double latitude_decimal, longitude_decimal;
  // Fixed point latitude and longitude value with degrees stored in units of 1/100000 degrees,
  // and minutes stored in units of 1/100000 degrees.  See pull #13 for more details:
  //   https://github.com/adafruit/Adafruit-GPS-Library/pull/13
int32_t latitude_fixed, longitude_fixed;
float latitudeDegrees, longitudeDegrees;
float geoidheight, altitude;
float speed, angle, magvariation, HDOP;
char lat, lon, mag;
bool fix;
uint8_t fixquality, satellites;

//bool waitForSentence(const char *wait, uint8_t max = MAXWAITSENTENCE);
bool LOCUS_StartLogger(void);
bool LOCUS_StopLogger(void);
bool LOCUS_ReadStatus(void);

uint16_t LOCUS_serial, LOCUS_records;
uint8_t LOCUS_type, LOCUS_mode, LOCUS_config, LOCUS_interval, LOCUS_distance, LOCUS_speed, LOCUS_status, LOCUS_percent;
bool paused;
  
uint8_t parseResponse(char *response);