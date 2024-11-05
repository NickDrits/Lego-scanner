#ifndef   _OUTPUT_H_
#define   _OUTPUT_H_

#define	  MAX_SPEED_FW (100)
#define	  MAX_SPEED_RW (-MAX_SPEED_FW)
#define   BRAKE    1
#define   FLOAT    0
#define NUM_MOTORS 3

typedef struct {
    SBYTE io_value;
} MotorConfig;

MotorConfig motor_config[NUM_MOTORS];

void      OutputInit(void);
void      OutputExit(void);
void      OutputSetSpeed (UBYTE MotorNr, SBYTE Speed);

#endif
