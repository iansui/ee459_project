#include "compass.h"
#include "i2c.h"
#include "serial.h"
#include <math.h>
#include <string.h>


uint8_t compass_init(){

    mag_address = 0x3C;//(0x3C >> 1);
    reg_mag_mr_reg_m = 0x02;
    reg_mag_cra_reg_m = 0x00;
    reg_mag_cra_reg_m = 0x01;


    uint8_t data = 0x00;

    uint8_t mag_status = i2c_io(mag_address, &reg_mag_mr_reg_m, 1, &data, 1, NULL, 0);

    uint8_t result = -1;
    mag_status = i2c_io(mag_address, &reg_mag_cra_reg_m, 1, NULL, 0, &result, 1);
	

    _lsm303Mag_Gauss_LSB_XY = 1100;
    _lsm303Mag_Gauss_LSB_Z = 980;

    uint8_t gain = 0x20;

     mag_status = i2c_io(mag_address, &reg_mag_crb_reg_m, 1, &gain, 0, NULL, 0);

    return mag_status;
}

uint8_t update_compass(){


    //  char compass_output_buff[256];


    uint8_t valid = 0;
    uint8_t result = -1;

    uint8_t index = 0x03;
    valid = valid | i2c_io(mag_address, &index, 1, NULL, 0, &result, 1);
    if(valid != 0){

       

        return 1;
    }
    uint8_t xhi = result;

    index = 0x04;
    valid = valid | i2c_io(mag_address, &index, 1, NULL, 0, &result, 1);
    if(valid != 0){

       

        return 1;
    }
    uint8_t xlo= result;

    index = 0x05;
    valid = valid | i2c_io(mag_address, &index, 1, NULL, 0, &result, 1);
    if(valid != 0){


      

        return 1;
    }
    uint8_t zhi = result;

    index = 0x06;
    valid = valid | i2c_io(mag_address, &index, 1, NULL, 0, &result, 1);
    if(valid != 0){


      

        return 1;
    }
    uint8_t zlo = result;

    index = 0x07;
    valid = valid | i2c_io(mag_address, &index, 1, NULL, 0, &result, 1);
    if(valid != 0){

       

        return 1;
    }
    uint8_t yhi = result;

    index = 0x08;
    valid = valid | i2c_io(mag_address, &index, 1, NULL, 0, &result, 1);
    if(valid != 0){

        return 1;
    }
    uint8_t ylo = result;

    raw_x = (int16_t)(xlo | ((int16_t)xhi << 8));
    raw_y = (int16_t)(ylo | ((int16_t)yhi << 8));
    raw_z = (int16_t)(zlo | ((int16_t)zhi << 8));

    mag_x = (float)raw_x / _lsm303Mag_Gauss_LSB_XY * 100;
    mag_y = (float)raw_y / _lsm303Mag_Gauss_LSB_XY * 100;
    mag_z = (float)raw_z / _lsm303Mag_Gauss_LSB_Z * 100;

    float mag_direction_f = atan2(mag_y, mag_x) * 180 / M_PI;

    mag_direction = (int16_t)mag_direction_f + 180;

    
    /*

    dtostrf(mag_x, 10, 7, mag_x_str);
    dtostrf(mag_y, 10, 7, mag_y_str);
    dtostrf(mag_z, 10, 7, mag_z_str);

    	snprintf(compass_output_buff, sizeof(compass_output_buff),
	 "raw_x: %i\r\n", raw_x);
		serial_string_out(compass_output_buff);
		memset(compass_output_buff, 0, sizeof(compass_output_buff));

        snprintf(compass_output_buff, sizeof(compass_output_buff),
	 "raw_y: %i\r\n", raw_y);
		serial_string_out(compass_output_buff);
		memset(compass_output_buff, 0, sizeof(compass_output_buff));

        snprintf(compass_output_buff, sizeof(compass_output_buff),
	 "raw_z: %i\r\n", raw_z);
		serial_string_out(compass_output_buff);
		memset(compass_output_buff, 0, sizeof(compass_output_buff));


        snprintf(compass_output_buff, sizeof(compass_output_buff),
	 "mag_x: %s\r\n", mag_x_str);
		serial_string_out(compass_output_buff);
		memset(compass_output_buff, 0, sizeof(compass_output_buff));


        snprintf(compass_output_buff, sizeof(compass_output_buff),
	 "mag_y: %s\r\n", mag_y_str);
		serial_string_out(compass_output_buff);
		memset(compass_output_buff, 0, sizeof(compass_output_buff));


        snprintf(compass_output_buff, sizeof(compass_output_buff),
	 "mag_z: %s\r\n", mag_z_str);
		serial_string_out(compass_output_buff);
		memset(compass_output_buff, 0, sizeof(compass_output_buff));


        snprintf(compass_output_buff, sizeof(compass_output_buff),
	 "mag_direction: %i\r\n", mag_direction);
		serial_string_out(compass_output_buff);
		memset(compass_output_buff, 0, sizeof(compass_output_buff));

        
    */

    return 0;

   

    

}

