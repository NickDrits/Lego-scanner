#include <stdlib.h>
#include <assert.h>
#include "AT91SAM7S256.h"
#include "hwinit.h"
#include "pit.h"
#include "aic.h"
#include "display.h"
#include "sound.h"
#include "i2c.h"
#include "input.h"
#include "button.h"
#include "output.h"
#include "led.h"

extern UBYTE ss ;
extern UBYTE printed ; 

int main(void) {
  UWORD value;
  UBYTE x=0,y=0 ,time = 0 ; 
  ULONG milisec , mili ,ticks; 
  milisec = (PIV * 32) /10 ; 
  mili = milisec ; 
  int buff[11][5];
  HardwareInit(); // need this to init PIOA clock
  
  DisplayInit(); 
  
  PITEnable();
  pit_init();

  AICInit();
  
  I2CInit();
  
  InputInit();
  ButtonInit();
  OutputInit();
  PITInterruptEnable(pit_handler);

  AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;

  UBYTE T = 0, z = 1 ;

  while(1) {

    I2CTransfer();

    if(z){
      if(ButtonRead() == 2){
        z = 0;
      }
    }
    else{
    if(ss %4 == 0 && ss != 0 && printed == 0){
      time = ss ; 
      InputGetSensorValue(&value, sensor_config[1].port);
      if(value >=570 && value <= 750 ){
        buff[y][x]=1;
      }
      else{
        buff[y][x] = 0 ; 
      }
      y++;
      if(y == 11){
        y = 0 ; 
        x++;
      }
      if(x ==5 ){
       break ; 
      }
      DisplayNum(10,50,ss);
      DisplayNum(60,50,value);
      DisplayNum(40,50,x);
      printed = 1 ; 
      if(y % 11 != 0 && y !=0){
        OutputSetSpeed(2,-61);
      }
      else {
        OutputSetSpeed(2,95);
        OutputSetSpeed(1 , -57);
        OutputSetSpeed(0 , -57);
      }
    }

    if(ss == time+1 || ss == time +2){
      OutputSetSpeed(0,0);
      OutputSetSpeed(1,0);
      OutputSetSpeed(2,0);
    }

    if(ButtonRead() == 4){
      I2CCtrl(REPROGRAM);
    }

    }
    DisplayUpdateSync();
    
  }
  DisplayErase();
  x = 10 ; 
  y = 10 ; 
  for(int i = 0 ; i <5 ; i ++){
    for(int j = 0 ; j < 11 ; j ++){
      if(buff[j][i] == 1){
        DisplaySetPixel(x,y);
        DisplaySetPixel(x+1,y);
        DisplaySetPixel(x,y+1);
        DisplaySetPixel(x+1,y+1);
      }
      x = x +2 ; 
    }
    x = 10 ; 
    y = y+2 ; 
  }
      DisplayUpdateSync();






  //ButtonExit();
  //InputExit();
  I2CExit();
  PITInterruptDisable();
  PITDisable();
  DisplayExit();

  return 0;
}
