#include "AT91SAM7S256.h"
#include  <stdconst.h>
#include  "arm2avr.h"
#include "input.h"

void InputInit(void) {
    sensor_config[0].port = 0;
    sensor_config[0].sensor_name = (UBYTE*)"Touch";
    IoFromAvr.AdValue[0]=0;
    
    sensor_config[1].port = 1;
    sensor_config[1].sensor_name = (UBYTE*)"Mic";
    IoFromAvr.AdValue[1]=0;
    
    sensor_config[2].port = 2;
    sensor_config[2].sensor_name = (UBYTE*)"Led";
    IoFromAvr.AdValue[2]=0;
    
    sensor_config[3].port = 3;
    sensor_config[3].sensor_name = (UBYTE*)"Light";
    IoFromAvr.AdValue[3]=0;
}

void InputGetSensorValue(UWORD *value, UBYTE port) {
    *value = IoFromAvr.AdValue[port];
}
