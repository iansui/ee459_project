#ifndef LOCATION_H
#define LOCARION_H

#include <stdlib.h>
#include <avr/pgmspace.h>


const char location_name_0[] PROGMEM = {"Viterbi E-quad Fountain"};
// const char location_data_0[] PROGMEM = {"This is a fountain."};
const char location_data_0[] PROGMEM = {"1This is a fountain.2This is a fountain.3This is a fountain.4This is a fountain.5This is a fountain.6This is a fountain.7This is a fountain.8This is a fountain.9This is a fountain.0This is a fountain.1This is a fountain.2This is a fountain.3This is a fountain.4This is a fountain.5This is a fountain.6This is a fountain.7This is a fountain.8This is a fountain."};
const char location_lat_0[] PROGMEM = {"34.020506"};
const char location_long_0[] PROGMEM = {"-118.289114"};

const char location_name_1[] PROGMEM = {"Neil Armstrong Statue"};
const char location_data_1[] PROGMEM = {"Hello Neil."};
const char location_lat_1[] PROGMEM = {"34.020134"};
const char location_long_1[] PROGMEM = {"-118.288520"};

const char ending[] PROGMEM = {"Thank you for playing. We hope you  enjoyed our EE459 project!"};

const char* const location_name_table[] PROGMEM = {location_name_0, location_name_1, ending};
const char* const location_data_table[] PROGMEM = {location_data_0, location_data_1};
const char* const location_lat_table[] PROGMEM = {location_lat_0, location_lat_1};
const char* const location_long_table[] PROGMEM = {location_long_0, location_long_1};

uint8_t location_index;
uint8_t location_size;

#endif
