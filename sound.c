#include <stdconst.h>
#include "AT91SAM7S256.h"
#include "sound.h"

/* Statically defined digitized sine wave, used for tone
 * generation.*/
static const ULONG tone_pattern[16] = {
  0xF0F0F0F0,0xF0F0F0F0,
  0xFCFCFCFC,0xFCFCFCFC,
  0xFFFFFFFF,0xFFFFFFFF,
  0xFCFCFCFC,0xFCFCFCFC,
  0xF0F0F0F0,0xF0F0F0F0,
  0xC0C0C0C0,0xC0C08080,
  0x00000000,0x00000000,
  0x8080C0C0,0xC0C0C0C0
};

static volatile UBYTE tone_lenght;

/* When a tone is playing, this value contains the number of times the
 * previous digitized sine wave is to be played.*/
static volatile ULONG tone_cycles;

void SoundIntDisable() {
  // Clear the interrupt enable bits in the SSC_IER register
  AT91C_BASE_SSC->SSC_IDR = AT91C_SSC_ENDTX;
}

void SoundDisable() {
  // Stop the transmission and reset the SSC peripheral
  AT91C_BASE_SSC->SSC_CR |= AT91C_SSC_SWRST | AT91C_SSC_RXDIS | AT91C_SSC_TXDIS;
  /*disable sound*/
  *AT91C_SSC_IDR = AT91C_SSC_ENDTX;
}

void SoundEnable(void) {
  *AT91C_SSC_IER = AT91C_SSC_ENDTX;    /* Enable end of transmit interrupt */
  *AT91C_SSC_CR = AT91C_SSC_TXEN;      /* Enable SSC transmitter */
}

void SoundInit(void) {
  SoundIntDisable();
  /* Start by inhibiting all sound output. Then enable power to the
   * SSC peripheral and do a software reset. 
   */
  *AT91C_PMC_PCER = (1 << AT91C_ID_SSC);
  *AT91C_SSC_IDR = ~0;
  *AT91C_SSC_CR = AT91C_SSC_SWRST;
  /* Configure the transmit clock to be based on the board master
   * clock, to clock continuously (don't stop sending a clock signal
   * when there is no data), and set transmissions to start as soon as
   * there is data available to send.
   */
  *AT91C_SSC_TCMR = (AT91C_SSC_CKS_DIV |
                     AT91C_SSC_CKO_CONTINOUS |
                     AT91C_SSC_START_CONTINOUS);
  /* Configure the framing mode for transmission: 32-bit data words, 8
   * words per frame, most significant bit first. Also set the default
   * driven value (when there is no data being streamed) to 1.
   */
  *AT91C_SSC_TFMR =
    31 | AT91C_SSC_DATDEF | AT91C_SSC_MSBF | (7 << 8);
  /* Idle the output data pin of the SSC. The value on the pin will
   * now be whatever the SSC pumps out.
   */
  *AT91C_PIOA_PDR = AT91C_PA17_TD;
  SoundEnable();
}

void SoundSync(UBYTE length, ULONG frequency, ULONG ms) {
  /* Due to a bug in the sound driver, too short a beep will crash
  * it. ms >= 100*/
  UBYTE i=0;
  SoundInit();
  *AT91C_SSC_CMR = ((96109714 / 1024) / frequency) + 1;
  tone_cycles = (frequency * ms) / 2000 - 1;
  while(tone_cycles-- > 0){
    for(i=0; i<length; i++){
      while(!(*AT91C_SSC_SR & AT91C_SSC_TXRDY)){/* SPIN */;};
      *AT91C_SSC_THR = tone_pattern[i];
    }
  }
  SoundDisable();
  return;
}

void bot_ (ULONG frequency, ULONG ms){
  SoundInit();
  *AT91C_SSC_CMR = ((96109714 / 1024) / frequency) + 1;
  tone_cycles = (frequency * ms) / 2000 - 1;
}

void SoundAsync(ULONG frequency, ULONG ms) {
  bot_(frequency, ms);
  *AT91C_SSC_THR = tone_pattern[0];
  tone_lenght = 1;
  AT91C_BASE_SSC->SSC_SR = AT91C_SSC_TXRDY;
}

void ssc_handler(void) {
  if (tone_cycles--) {
    //if(tone_lenght<16){
      *AT91C_SSC_THR = tone_pattern[tone_lenght];
      tone_lenght++;
      AT91C_BASE_SSC->SSC_SR = AT91C_SSC_TXRDY;
      // or AT91C_SSC_RXRDY
    //}
  }
  else {
    /* Transmit complete, disable sound again. */
    SoundDisable();
  }
}

/*void SSCInterruptEnable(void (*handler)(void)){
    AICInterruptEnable(AT91C_ID_SSC,handler);
}*/
