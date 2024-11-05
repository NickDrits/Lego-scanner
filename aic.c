#include  "aic.h"
#include  "pit.h"
#include "AT91SAM7S256.h"

void AICInit(void)
{
    //AT91C_BASE_AIC->AIC_IDCR = 1 << AT91C_ID_SYS;  // Disable SYS interrupt
    AT91C_BASE_AIC->AIC_IDCR = 0xFFFFFFFF;
    //0xFFFFFFFD
}

void AICInterruptEnable(unsigned int which, void (*handler)(void)){
    AT91C_BASE_AIC->AIC_SVR[which] = (unsigned int) handler; // Set ISR address
    AT91C_BASE_AIC->AIC_SMR[which] = AT91C_AIC_SRCTYPE_EXT_HIGH_LEVEL | 0x4; // Set interrupt mode
    AT91C_BASE_AIC->AIC_ICCR = 1 << which;  // Clear interrupt
    AT91C_BASE_AIC->AIC_IECR = 1 << which;  // Enable interrupt
}

void AICInterruptDisable(unsigned int which){
    AT91C_BASE_AIC->AIC_ICCR = 1 << which;  // Clear interrupt
    AT91C_BASE_AIC->AIC_IDCR = 0;
    AT91C_BASE_AIC->AIC_IDCR = 1 << which;  // Enable interrupt
}
