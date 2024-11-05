#ifndef   PIT
#define   PIT

#include <stdconst.h>
#define MCK             48000000        //47923200 Master clock frequency in Hz
#define PIV             ((MCK/(16*32))-1)            // PIT period in milliseconds

void      PITEnable(void);
void      PITDisable(void);
ULONG     PITRead(void);
void      PITInterruptDisable(void);
void      PITAckInterrupt(void);
UWORD	  PITTicks2ms(ULONG ticks);
UWORD	  PITTicks2s(ULONG ticks);
void 	  spindelayms(ULONG ms);
ULONG     PITReadCPIV(void);
void      pit_init(void);
void      (*pit_handler(void));
void      PITReadReset(void);
void      PITInterruptEnable(void (*handler)(void));

#endif
