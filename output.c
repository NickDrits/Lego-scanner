#include "AT91SAM7S256.h"
#include  <stdconst.h>
#include  "arm2avr.h"
#include "output.h"

void OutputSetSpeed(UBYTE MotorNr, SBYTE Speed) {
    motor_config[MotorNr].io_value = Speed;
    IoToAvr.PwmValue[MotorNr] = Speed;
    IoToAvr.OutputMode = FLOAT;
}

void OutputInit(void) {
    IoToAvr.Power = 0;
    IoToAvr.PwmFreq = 8;
    IoToAvr.PwmValue[0] = 0;
    IoToAvr.PwmValue[1] = 0;
    IoToAvr.PwmValue[2] = 0;
    IoToAvr.PwmValue[3] = 0;
}
