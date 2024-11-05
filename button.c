#include "AT91SAM7S256.h"
#include  <stdconst.h>
#include  "arm2avr.h"
#include "button.h"

void   ButtonInit(void){
    IoFromAvr.Buttons = (UWORD)0x0000;
}
void   ButtonExit(void){

}
enum button_t ButtonRead(void){    
    if(IoFromAvr.Buttons == 2047){
        return BUTTON_ENTER;
    }
    if(IoFromAvr.Buttons == 407){
        return BUTTON_RIGHT;
    }
    if(IoFromAvr.Buttons == 127){
        return BUTTON_LEFT;
    }
    if(IoFromAvr.Buttons == 1023){
        return BUTTON_EXIT;
    }
    return BUTTON_NONE;
}
