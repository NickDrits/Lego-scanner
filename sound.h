#ifndef   SOUND
#define   SOUND

#include <stdconst.h>

void SoundInit(void);
void SoundSync(UBYTE length, ULONG frequency, ULONG ms);
void SoundAsync(ULONG frequency, ULONG ms);
void SoundExit(void);
void ssc_handler(void);

#endif
