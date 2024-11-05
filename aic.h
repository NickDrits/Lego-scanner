#ifndef   AIC
#define   AIC

#include <stdconst.h>

void      AICInit(void);
void      AICInterruptEnable(unsigned int which, void (*handler)(void));
void      AICInterruptDisable(unsigned int which);

#endif
