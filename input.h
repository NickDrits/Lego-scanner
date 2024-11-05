#ifndef   _INPUT_H
#define   _INPUT_H

#define NUM_INPUT_PORTS 4

typedef struct {
    UBYTE port;
    UBYTE* sensor_name;
} SensorConfig;

SensorConfig sensor_config[NUM_INPUT_PORTS];

void InputInit(void);
void InputExit(void);
void InputGetSensorValue(UWORD *value, UBYTE port);

#endif
