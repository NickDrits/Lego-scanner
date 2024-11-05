#ifndef   _SPI_H_
#define   _SPI_H_

#include <stdconst.h>

void SPIInit(void);
unsigned int SPITxReady(void);
unsigned int SPIRxReady(void);
void SPIWrite(UBYTE *buf, UBYTE len);
void SPIRead(UBYTE *buf, UBYTE len);
void SPIPIOSetData(void);
void SPIPIOClearData(void);

#endif
