#ifndef GPS_H
#define GPS_H

uint8_t hour, minute, seconds, year, month, day;
uint16_t milliseconds;

float latitude, longitude;
float lat_comp, long_comp;
double latitude_decimal, longitude_decimal;
int32_t latitude_fixed, longitude_fixed;
float latitudeDegrees, longitudeDegrees;

float geoidheight, altitude;
float speed, angle, magvariation, HDOP;
char lat, lon, mag;
bool fix;
uint8_t fixquality, satellites;

bool paused;

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
uint8_t parseResponse(char *response);

#endif