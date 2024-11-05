#include  "pit.h"
#include  "aic.h"
#include  "sound.h"
#include  "display.h"
#include "AT91SAM7S256.h"

UBYTE ss = 0,mm = 0,hh = 0 ;
UBYTE printed = 0 ; 

void PITEnable(void){
    *AT91C_PITC_PIMR = AT91C_PITC_PITEN; 
}

void PITDisable(void){
    *AT91C_PITC_PIMR = ((unsigned int) 0x0 << 24); 
}

ULONG PITReadCPIV(void){
    ULONG a = (ULONG)(*AT91C_PITC_PIIR & ((1u << 20) - 1));
    ULONG b = (ULONG)((AT91C_BASE_PITC->PITC_PIIR >> (21-1)) & ((1 << 12) - 1));
    b = b * PIV;
    ULONG sum = a + b;
    return sum;
}

void PITReadReset(void){
    ULONG a = (ULONG)(AT91C_BASE_PITC->PITC_PIVR);
    a = 0;
}

UWORD PITTicks2ms(ULONG ticks){
    ULONG ms = ticks / (32*PIV);
    ms = ms * 1000;
    ULONG mss = ticks % (32*PIV);
    mss = mss * 0.0004;
    ULONG sum = mss + ms;
    return sum;
}

UWORD PITTicks2s(ULONG ticks){
    ticks = ticks / (32*PIV);
    return ticks;
}

void pit_init(void)
{
    // Enable PIT clock
    AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_SYS;

    // Configure PIT
    AT91C_BASE_PITC->PITC_PIMR = AT91C_PITC_PITEN    // Enable PIT
                             | AT91C_PITC_PITIEN 
                             | PIV;   // Set PIT period
    /*
      *AT91C_PITC_PIMR = (AT91C_PITC_PITIEN | 
		                  AT91C_PITC_PITEN  |   
		                  PIV);
    */
}

void (*pit_handler(void)){
    ULONG tick = PITReadCPIV();
    if(tick/PIV == 32){
      PITReadReset();
      ss++;
      if(ss%2 == 0 && ss != 0 ){
        printed = 0 ; 
      }
    }
}

void PITInterruptEnable(void (*handler)(void)){
    AICInit();
    AICInterruptEnable(AT91C_ID_SYS,handler);
    AICInterruptEnable(AT91C_ID_SSC,ssc_handler);
}

void PITInterruptDisable(void){
    *AT91C_PITC_PIMR = (unsigned int) 1 << 26;
}
