#include "AT91SAM7S256.h"
#include  <stdconst.h>
#include  <assert.h>
#include  <string.h>
#include  <stdlib.h>
#include  "i2c.h"
#include  "arm2avr.h"
#include  "aic.h"
#include  "display.h"

#define BYTES_TO_TX     sizeof(IOTOAVR)
#define BYTES_TO_RX     sizeof(IOFROMAVR)+1
#define TIMEOUT         2000
#define I2CClk          400000L
#define TIME400KHz      (((OSC/16L)/(I2CClk * 2)) + 1)
#define CLDIV           (((OSC/I2CClk)/2)-3)
#define DEVICE_ADR      0x01
#define HOAX "Let's samba nxt arm in arm, (c)LEGO System A/S"
#define COPYRIGHTSTRINGLENGTH 47
const   UBYTE FAKE[] = {"\xCC"HOAX};
const   UBYTE CopyrightStr[47] = {0xCC,0x4C,0x65,0x74,0x27,0x73,0x20,0x73,0x61,0x6D,0x62,0x61,0x20,
                                0x6E,0x78,0x74,0x20,0x61,0x72,0x6D,0x20,0x69,0x6E,0x20,0x61,0x72,0x6D,
                                0x2C,0x20,0x28,0x63,0x29,0x4C,0x45,0x47,0x4F,0x20,0x53,0x79,0x73,0x74,
                                0x65,0x6D,0x20,0x41,0x2F,0x53 };
#define MAX(x,y) (((x)>(y))?(x):(y))
#define piir (*AT91C_PITC_PIIR & AT91C_PITC_CPIV)
#define pimr (*AT91C_PITC_PIMR & AT91C_PITC_CPIV)
#define DISABLEI2cIrqs *AT91C_TWI_IDR = 0x000001C7

#define WAITClk(t) {\
	  ULONG pit = piir + (t);\
          if (pit >= pimr) pit -= pimr;\
          while (piir < pit){;}\
        }

// send/receive state machine variables WAS STATIC
enum state_t {RESET, SENDING, SEND, RECV, RECEIVING}; 
static enum state_t volatile State = RESET;
UBYTE volatile counter;
UBYTE volatile Checksum;
UBYTE volatile lenght;
UBYTE volatile RECVBUFFER[14];
UBYTE volatile SENDBUFFER[48];
UBYTE *BUFFER;
/*
void DataTxInit(UBYTE *buf, UBYTE len) {
  BUFFER = (UBYTE*) malloc(sizeof(UBYTE) * (len+1));
  UBYTE i = 0;//kaue 2 byte
  UBYTE checksum = 0;
  lenght = 0;
  for(i=0;i<len;i++){
    BUFFER[i] = buf[i];
    checksum += ~buf[i];
  }
  lenght = len + 1;
  BUFFER[len] = checksum;
  AT91C_BASE_TWI->TWI_MMR = ((unsigned int) 0x0 << 12) |((unsigned int) DEVICE_ADR << 16);
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_MSEN | AT91C_TWI_START;
  AT91C_BASE_TWI->TWI_IER = AT91C_TWI_TXRDY | AT91C_TWI_NACK;
  AICInterruptEnable(AT91C_ID_TWI,I2cHandler);
  AT91C_BASE_TWI->TWI_THR = BUFFER[0];
  counter = 1;
  return;
}  */

void DataTxInit(UBYTE *buf, UBYTE len) {
  UBYTE i = 0;
  UBYTE checksum = 0;
  lenght = 0;
  for(i=0;i<len;i++){
    SENDBUFFER[i] = buf[i];
    checksum += buf[i];
  }
  checksum = ~checksum ; 
  lenght = len + 1;
  SENDBUFFER[len] = checksum;
  AT91C_BASE_TWI->TWI_MMR = ((unsigned int) 0x0 << 12) |((unsigned int) DEVICE_ADR << 16);
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START | ((unsigned int) 0x1 << 5) ; 
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_MSEN; 
  AT91C_BASE_TWI->TWI_IER = AT91C_TWI_TXRDY | AT91C_TWI_NACK | AT91C_TWI_TXCOMP;
  AICInterruptEnable(AT91C_ID_TWI,I2cHandler);
  AT91C_BASE_TWI->TWI_THR = SENDBUFFER[0];
  counter = 1;
  return;
}

void DataRxInit(void) {
  Checksum = 0;
  counter = 0;
  AT91C_BASE_TWI->TWI_MMR = ((unsigned int) 0x1 << 12) |((unsigned int) DEVICE_ADR << 16);
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START | ((unsigned int) 0x1 << 5);
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_MSEN; 
  AT91C_BASE_TWI->TWI_IER = AT91C_TWI_RXRDY | AT91C_TWI_NACK |AT91C_TWI_TXCOMP;
  AICInterruptEnable(AT91C_ID_TWI,I2cHandler);
  return;
}

void I2cHandler(void){
  if(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_NACK){
    State = RESET;
    DISABLEI2cIrqs;
    DisplayString(30,30,(UBYTE*)"NACK");
    DisplayUpdateSync();
  }
  if(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_RXRDY){
    if(counter<BYTES_TO_RX-1){
      RECVBUFFER[counter] = (UBYTE)AT91C_BASE_TWI->TWI_RHR;
      Checksum += RECVBUFFER[counter];
      counter++;
    }
    else if(counter == (BYTES_TO_RX-1)){
      RECVBUFFER[counter] = (UBYTE)AT91C_BASE_TWI->TWI_RHR;
      Checksum += RECVBUFFER[counter];  
      counter++;
      AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP;
    }
    else if(counter == BYTES_TO_RX){
      Checksum = ~Checksum ; 
      State = SEND;

      //AT91C_BASE_TWI->TWI_IDR = AT91C_TWI_RXRDY;
      //AICInterruptDisable(AT91C_ID_TWI);
      RECVBUFFER[13] = (UBYTE)AT91C_BASE_TWI->TWI_RHR;

      if(Checksum == RECVBUFFER[13]){
        IoFromAvr.AdValue[0] = RECVBUFFER[2] << 8 | RECVBUFFER[1];
        IoFromAvr.AdValue[1] = RECVBUFFER[4] << 8 | RECVBUFFER[3];
        IoFromAvr.AdValue[2] = RECVBUFFER[6] << 8 | RECVBUFFER[5];
        IoFromAvr.AdValue[3] = RECVBUFFER[8] << 8 | RECVBUFFER[7];
        IoFromAvr.Buttons = RECVBUFFER[10] << 8 | RECVBUFFER[9];
        IoFromAvr.Battery = RECVBUFFER[12] << 8 | RECVBUFFER[11]; 
        DISABLEI2cIrqs;
      }
      else{
        State = RESET ; 
        DISABLEI2cIrqs;
      }
    }
  }
  if(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXRDY){
    if(counter<(lenght-1)){//ANY
      AT91C_BASE_TWI->TWI_THR = SENDBUFFER[counter];
      counter++;
    }
    else if(counter == (lenght-1)){//LAST
      AT91C_BASE_TWI->TWI_THR = SENDBUFFER[counter];
      counter++;
      AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP;
    }
    else if(counter == lenght){//AFTER LAST
      //AT91C_BASE_TWI->TWI_IDR = AT91C_TWI_TXRDY;
      //AICInterruptDisable(AT91C_ID_TWI);
      //free(BUFFER);
      DISABLEI2cIrqs;
      State = RECV;
    }
  }
  if(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP){
  }
}

void I2CTransfer(void) {
  WAITClk(TIMEOUT);
  switch (State)
  {
  case SEND:
    DataTxInit((UBYTE*)(&IoToAvr), BYTES_TO_TX);
    State = SENDING;
    break;

  case RECV:
    DataRxInit();
    State = RECEIVING;
    break;
  /*
  case INIT:
    I2CInit();
    State = RESET;
    break;
  */
  case RESET:
    DataTxInit((UBYTE*)CopyrightStr, COPYRIGHTSTRINGLENGTH);
    State = SENDING;
    break;

  case SENDING:
    break;

  case RECEIVING:
    break;
  }
  return;
}

void I2CCtrl (enum power_t p) {
  if(p == NORMAL_OP){
    IoToAvr.Power = p;
  }
  else if(p == POWERDOWN){
    IoToAvr.Power = 0x5A;
    IoToAvr.PwmFreq = 0x00;
  }
  else if(p == REPROGRAM){
    IoToAvr.Power = 0xA5;
    IoToAvr.PwmFreq = 0x5A;
  }
  return;
}

void I2CInit(void) { 
  
  //
  // disable I2C on PIO
  // this is called also during an error, so interrupts etc may be enabled
  //
  *AT91C_AIC_IDCR = (1L<<AT91C_ID_TWI);			/* disable AIC irq  */
  DISABLEI2cIrqs;                      			/* disable TWI irq  */
  *AT91C_PMC_PCER  = (1L<<AT91C_ID_TWI);		/* enable TWI Clock */
  *AT91C_PIOA_OER  = AT91C_PA4_TWCK;  		  	/* SCL is output    */
  *AT91C_PIOA_ODR  = AT91C_PA3_TWD;			/* SDA is input     */
  *AT91C_PIOA_MDER = (AT91C_PA4_TWCK | AT91C_PA3_TWD);  /* open drain       */
  *AT91C_PIOA_PPUDR = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* no pull up       */
  
  // 
  // synch I2C clocks using PIO
  // generate a 400KHz pulse on SCK and wait until both SCK and SDA are high, 
  // which means the slave ticks with this clock
  //
  *AT91C_PIOA_PER  = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* enable PIO control for these pins */
  while(((*AT91C_PIOA_PDSR & AT91C_PA3_TWD) == 0) || ((*AT91C_PIOA_PDSR & AT91C_PA4_TWCK) == 0)){
      *AT91C_PIOA_CODR = AT91C_PA4_TWCK; /* drive SCL Low  */
      WAITClk(TIME400KHz);
      *AT91C_PIOA_SODR = AT91C_PA4_TWCK; /* drive SCL High */
      WAITClk(TIME400KHz);
  }
  // 
  // init I2C on PIO
  //
  *AT91C_TWI_CR    =  AT91C_TWI_SWRST;			/* this has to happen before the rest */
  *AT91C_PIOA_PDR   = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* disable PIO control for these pins */
  *AT91C_PIOA_ASR   = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* select peripheral A = TWI */
  *AT91C_TWI_CWGR   = (CLDIV | (CLDIV << 8));           /* 400KHz clock    */
  *AT91C_AIC_ICCR   = (1L<<AT91C_ID_TWI);               /* clear AIC irq   */
  AT91C_AIC_SVR[AT91C_ID_TWI] = (unsigned int)I2cHandler;
  AT91C_AIC_SMR[AT91C_ID_TWI] = ((AT91C_AIC_PRIOR_HIGHEST) | (AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED));
  *AT91C_AIC_IECR   = (1L<<AT91C_ID_TWI);               /* Enables AIC irq */
  
  IoToAvr.Power     = 0;
  
  return;
}

void I2CExit(void) {
  DISABLEI2cIrqs;
  *AT91C_AIC_IDCR   = (1L<<AT91C_ID_TWI);               /* Disable AIC irq  */
  *AT91C_AIC_ICCR   = (1L<<AT91C_ID_TWI);               /* Clear AIC irq    */
  *AT91C_PMC_PCDR   = (1L<<AT91C_ID_TWI);               /* Disable clock    */
  *AT91C_PIOA_MDER  = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* Open drain       */
  *AT91C_PIOA_PPUDR = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* no pull up       */
  *AT91C_PIOA_PER   = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* Disable peripheal*/
}
