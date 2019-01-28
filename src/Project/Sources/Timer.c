#include <MC9S08AW16.h>
#include "Timer.h"
#include "IO_HWL.h"
#include "ADC_handler.h"
#include "Button_handler.h"
#include "Behaviour.h"

extern Buzz_en BuzzState;
extern dword BuzzerTimer;
extern ControlState_en Behaviour_state;
//extern PEN_State_en Pointer_State;
//extern dword PEN_Debounce_Time;

extern word DisplayCount;
extern XPOS_State_en XposeState;

extern JawConfig_st TimerConfig;
extern SelectedVal_en Selection;
extern MemConfig_st * ptr_MemConfg;
extern byte id;
extern const byte XPOS_PauseTimerId;
extern dword Timer_id_CurrVal;

const word TPM1_period = (5529/2);//Configured for 1ms

const word TPM2_period = (5529/2);

dword TimerOverFlowCount = DISPLAY_FREQ;
dword ThreadTimerOFCount = 0;
word ThreadTimeCounter = 0;
dword XPOS_TimerOVCount = 0;


//Uses port pin PTD bit 3
void Init_Timer() {  
  
  TPM2MODH = TPM2_period/0x100;
  TPM2MODL = TPM2_period & 0xff;    
  
  //Initialize the count
  TPM2CNTH = 0;
  TPM2CNTL = 0;     
  
  TPM2C1SC = 0;//No pin attached to timer channel  
  TPM2SC = 0x08;//Clock select: Bus clock   
  TPM2SC_TOIE = 1;//overflow Interrupt enable  
}

//Timer for X-Ray expose
void Init_XPOS_Timer() {
  TPM1MODH = TPM1_period/0x100;
  TPM1MODL = TPM1_period & 0xff;
  TPM1CNTH = 0;
  TPM1CNTL = 0;     
  TPM1C1SC = 0;//No pin attached to timer channel
  //TPM1SC = 0x08;//Clock select: Bus clock   
  //TPM1SC_TOIE = 1;//overflow Interrupt enable  
}

interrupt VectorNumber_Vtpm1ovf ISR_TPM1_TimerOverflow() {
//ISR duration = 240us
  byte dummy;
  
  dummy = TPM1SC;
  TPM1SC &= 0x7f;
    
  TPM1CNTH = 0;
  TPM1CNTL = 0; 
  
  
  if(XPOS_TimerOVCount > 0) {
    TPM1MODH = TPM1_period/0x100;
    TPM1MODL = TPM1_period & 0xff;
    XPOS_TimerOVCount--;
  
  //Decrement the count on the LEDs
    if(XPOS_TimerOVCount <= DisplayCount)
    {     
      DisplayCount--;
    }
  } else {
    PTDD &= 0xfb;//Stop the XPOS
    //PTCD &= 0xdf;//Switch Off the EXP led.
    ChangeLEDState(LED_EXP,0);//Expose
    //Stop the timer and 
    TPM1SC = 0;
    TPM1SC_TOIE = 0;    
    
    BuzzState = BUZZ_OFF;//Stop Buzzer            
    DisplayCount = 0;
        
      //Display pause time
      id = XPOS_PauseTimerId;
      XposeState = XPOS_IDLE_BEFORE_PAUSE;
      Timer_id_CurrVal = 250;
      StartThreadTimer(0,0);
      
      
  }
  
   
}

void StartXPOSTimer(dword time) {
  
  word temp = 0;
  TPM1MODH = TPM1_period/0x100;
  TPM1MODL = TPM1_period & 0xff;
  TPM1CNTH = 0;
  TPM1CNTL = 0;     
  TPM1C1SC = 0;//No pin attached to timer channel
  
  temp = (DisplayCount - ((DisplayCount * 3)/100)) + (TimerConfig.PauseTime - ((TimerConfig.PauseTime * 3)/100));//time;
  XPOS_TimerOVCount = temp;
  
  TPM1SC = 0x08;//Clock select: Bus clock   
  TPM1SC_TOIE = 1;//overflow Interrupt enable  
  
  PTDD |= 0x04; //Set expose//PD2
  //Set the EXP led
  //PTCD |= 0x20;//PC5
  ChangeLEDState(LED_EXP,1);//Expose
}

void StopXPOSTimer()
{
  //Stop the timer and 
    TPM1SC = 0;
    TPM1SC_TOIE = 0;
    PTDD &= 0xfb;//Stop the XPOS
    //PTCD &= 0xdf;//Switch Off the EXP led.
    ChangeLEDState(LED_EXP,0);//Expose
    ChangeLEDState(LED_START,0);//Start
    BuzzState = BUZZ_OFF;//Stop Buzzer    
    Behaviour_state = STOP_SET;
    XposeState = XPOS_IDLE;
    XPOS_TimerOVCount = 0;
    
}

interrupt VectorNumber_Vtpm2ovf ISR_TPM2_TimerOverflow() {

  byte dummy = TPM2SC;
  TPM2SC &= 0x7f;
  TPM2CNTH = 0;
  TPM2CNTL = 0;
  
  TPM2SC = 0x08;
  TPM2SC_TOIE = 1;  
  
  if(TimerOverFlowCount > 0) {
    
    TimerOverFlowCount--;
    if(TimerOverFlowCount == 0) {
      DisplayHMI();      
      TimerOverFlowCount = DISPLAY_FREQ;      
    }
    
  }
  if(BuzzState != BUZZ_OFF) {
    if(BuzzerTimer != 0) {
      BuzzerTimer--;
    } 
  } 
  SetBuzzer();
  /*if(Pointer_State == PEN_ACTIVE) {
    if(PEN_Debounce_Time > 0) {
      
      PEN_Debounce_Time--;
    }
  }*/
  if(ThreadTimeCounter == 0xffff) {
    ThreadTimerOFCount++;
  }
  ThreadTimeCounter++;
  if(XposeState == XPOS_PAUSE_TIME)
  {
     DisplayCount = DisplayCount - (DISPLAY_FREQ);
  }

}

//Disable Timer by assigning no clock
void DisableTimer() {
  TPM2SC &= 0xe7;//Sets CLKS to 00
  
}

//Start timer for a given count
void EnableTimer(word count) {
  
  TPM2CNTH = 0;
  TPM2CNTL = 0;
  
  TPM2MODH = count/0x100;
  TPM2MODL = count & 0xff;  
  
  TPM2SC |= 0x08;//Clock select: Bus clock   
  TPM2SC_TOIE = 1;//overflow Interrupt enable
  
}

//Interface for starting timer with given time in ms
byte StartTimer(word time) {
  //1ms = 5529 counts
  //time ms = ?
  //dword count = TPM1_period * time;
  byte ret_val = 0;
  if(TimerOverFlowCount == 0) {
    
    TimerOverFlowCount = time;    
    ret_val = 1;
  }              
  return ret_val;
  
}