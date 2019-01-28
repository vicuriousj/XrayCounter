#include <MC9S08AW16.h>
#include "Utilities.h"


void InitClock(void) {
 
 //Configuration using an external clock of 11.0592MHz.
 
 
 //Configuring for 5.5MHz ICGOUT
 //ICGOUT = ICGDCLK/R
 //ICGOUT = (11.0592MHz)*(P = 1)*(N=4/R=8)
 
 //Enable oscillator using internal resonator, mode = 10 i.e,FLL bypassed, external reference
 ICGC1 = 0x70;//0x30; 
 
 //Multiplication factor = 0 and division factor = 2.(0x01)
 ICGC2 = 0x01;     
 Delay(2);
  
 //whlie DCO is cleared
 while(!(ICGS2 & 0x01)) {
    __RESET_WATCHDOG();
 }
 
  //Set FLASH clock
  //FCDIV
  //DivisorLoadedFlag(7R), Prescale divide(6), Divisor for Flashclock divider(5-0)  
  //Bus clock = 5529KHz.
  //5529/28 = 197KHz, value should be divisor -1
  if(!(FCDIV & 0x80))
  {
    FSTAT = 0x30;
    FCDIV = 27;
  }
  
  
}   


/*delay function*/
void Delay(word val) {
//1ms = 5529ticks
word ms_val = val;

  while(ms_val > 0) {    
    
    word delay_val = 10;
    while(delay_val > 0) {
      __RESET_WATCHDOG();      
      delay_val--;
    }
    ms_val--;
  }
} 

