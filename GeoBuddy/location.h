/*
EE459 Spring 2019
Team 15
Project name: GeoBuddy
Ling Ye
Brian Suitt
Yi Sui
*/

/*
location.h contains the location data that is used by the main program
*/

#ifndef LOCATION_H
#define LOCARION_H

#include <stdlib.h>
#include <avr/pgmspace.h>

const char location_name_0[] PROGMEM = {"Viterbi E-quad Fountain"};
const char location_data_0[] PROGMEM = {"This is the center of the Viterbi Engineering School. This is the place where engineering students hang out and work on their projects."};
const char location_lat_0[] PROGMEM = {"34.020506"};
const char location_long_0[] PROGMEM = {"-118.289114"};

const char location_name_1[] PROGMEM = {"Science & Engineering Library"};
const char location_data_1[] PROGMEM = {"Formed in 1970 by the merger of USC’s existing science and engineering libraries, the Science & Engineering Library today provides essential support to student, faculty, and staff researchers in the life sciences, physical sciences, earth sciences, mathematics, and engineering."};
const char location_lat_1[] PROGMEM = {"34.019754"};
const char location_long_1[] PROGMEM = {"-118.288588"};

const char location_name_2[] PROGMEM = {"Neil Armstrong Statue"};
const char location_data_2[] PROGMEM = {"Armstrong was an American astronaut and aeronautical engineer who was the first person to walk on the Moon. He was also a naval aviator, test pilot, and university professor. In 1970 he completed his Master of Science degree in Aerospace Engineering at USC."};
const char location_lat_2[] PROGMEM = {"34.020134"};
const char location_long_2[] PROGMEM = {"-118.288520"};

const char location_name_3[] PROGMEM = {"Electrical Engineering Center"};
const char location_data_3[] PROGMEM = {"The Hughes Aircraft Electrical Engineering Center houses programs of the USC Viterbi School of Engineering's Ming Hsieh Department of Electrical Engineering. It is also home to the USC Stevens Institute for Innovation."};
const char location_lat_3[] PROGMEM = {"34.019828"};
const char location_long_3[] PROGMEM = {"-118.289974"};

const char ending[] PROGMEM = {"Thank you for playing. We hope you  enjoyed our EE459 project!"};

const char* const location_name_table[] PROGMEM = {location_name_0, location_name_1, location_name_2, location_name_3, ending};
const char* const location_data_table[] PROGMEM = {location_data_0, location_data_1, location_data_2, location_data_3};
const char* const location_lat_table[] PROGMEM = {location_lat_0, location_lat_1, location_lat_2, location_lat_3};
const char* const location_long_table[] PROGMEM = {location_long_0, location_long_1, location_long_2, location_long_3};

uint8_t location_index;
uint8_t location_size;

#endif
