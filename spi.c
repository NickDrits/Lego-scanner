#include "AT91SAM7S256.h"
#include "spi.h"

#define SPI_BITRATE 2000000

void SPIInit(void)
{
	*AT91C_PMC_PCER = (1L << AT91C_ID_SPI); /* Enable MCK clock     */
	// map the multiplexed pins of PIO to SPI
	*AT91C_PIOA_PER = AT91C_PIO_PA12; /* Enable A0 on PA12    */
	*AT91C_PIOA_OER = AT91C_PIO_PA12;
	*AT91C_PIOA_CODR = AT91C_PIO_PA12;
	*AT91C_PIOA_PDR = AT91C_PA14_SPCK; /* Enable SPCK on PA14  */
	*AT91C_PIOA_ASR = AT91C_PA14_SPCK;
	*AT91C_PIOA_ODR = AT91C_PA14_SPCK;
	*AT91C_PIOA_OWER = AT91C_PA14_SPCK;
	*AT91C_PIOA_MDDR = AT91C_PA14_SPCK;
	*AT91C_PIOA_PPUDR = AT91C_PA14_SPCK;
	*AT91C_PIOA_IFDR = AT91C_PA14_SPCK;
	*AT91C_PIOA_CODR = AT91C_PA14_SPCK;
	*AT91C_PIOA_IDR = AT91C_PA14_SPCK;
	*AT91C_PIOA_PDR = AT91C_PA13_MOSI; /* Enable mosi on PA13  */
	*AT91C_PIOA_ASR = AT91C_PA13_MOSI;
	*AT91C_PIOA_ODR = AT91C_PA13_MOSI;
	*AT91C_PIOA_OWER = AT91C_PA13_MOSI;
	*AT91C_PIOA_MDDR = AT91C_PA13_MOSI;
	*AT91C_PIOA_PPUDR = AT91C_PA13_MOSI;
	*AT91C_PIOA_IFDR = AT91C_PA13_MOSI;
	*AT91C_PIOA_CODR = AT91C_PA13_MOSI;
	*AT91C_PIOA_IDR = AT91C_PA13_MOSI;
	*AT91C_PIOA_PDR = AT91C_PA10_NPCS2; /* Enable npcs0 on PA11  */
	*AT91C_PIOA_BSR = AT91C_PA10_NPCS2;
	*AT91C_PIOA_ODR = AT91C_PA10_NPCS2;
	*AT91C_PIOA_OWER = AT91C_PA10_NPCS2;
	*AT91C_PIOA_MDDR = AT91C_PA10_NPCS2;
	*AT91C_PIOA_PPUDR = AT91C_PA10_NPCS2;
	*AT91C_PIOA_IFDR = AT91C_PA10_NPCS2;
	*AT91C_PIOA_CODR = AT91C_PA10_NPCS2;
	*AT91C_PIOA_IDR = AT91C_PA10_NPCS2;
	// init the SPI
	*AT91C_SPI_CR = AT91C_SPI_SWRST; /* Soft reset           */
	*AT91C_SPI_CR = AT91C_SPI_SPIEN; /* Enable spi           */
	*AT91C_SPI_MR = AT91C_SPI_MSTR | ((unsigned int)0x1 << 0) | AT91C_SPI_MODFDIS | (0xB << 16);
	AT91C_SPI_CSR[2] = ((OSC / SPI_BITRATE) << 8) | AT91C_SPI_CPOL;

	return;
}

void SPIPIOSetData(void)
{
	AT91C_BASE_PIOA->PIO_SODR = AT91C_PIO_PA12; 
}

void SPIPIOClearData(void) {
    AT91C_BASE_PIOA->PIO_CODR = AT91C_PIO_PA12; 
}

unsigned int SPITxReady(void)
{
	return ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) && 0x1);
}

unsigned int SPIRxReady(void)
{
	return ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF) && 0x1);
}

void SPIWrite(UBYTE *buf, UBYTE len)
{
	while (len--)
	{
		// Wait until the transmit data register is ready
		while (!SPITxReady());

		// Write the byte to the transmit data register
		AT91C_BASE_SPI->SPI_TDR = *buf++;
	}
}

/*With PDC*/
void SPIWriteDMA(UBYTE *buf, UBYTE len){
    *AT91C_SPI_TPR = (unsigned int)buf;
    *AT91C_SPI_TCR = (unsigned int)len;
	*AT91C_SPI_PTCR = AT91C_PDC_TXTEN;
}

void SPIRead(UBYTE *buf, UBYTE len)
{
	while (len--)
	{
		// Wait until the receive data register is ready
		while (!SPIRxReady());

		// Read the received byte from the receive data register
		*buf++ = AT91C_BASE_SPI->SPI_RDR;
	}
}
