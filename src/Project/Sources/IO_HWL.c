#include <MC9S08AW16.h>
#include "IO_HWL.h"
#include "ADC_handler.h"
#include "Button_handler.h"
#include "Behaviour.h"

//PE4 - PENIRQ 
const byte Ports[] = {PORT_A, PORT_B, PORT_C, PORT_D, PORT_E, PORT_F, PORT_G};
const byte PORT_COUNT = 7;

//byte PEN_TOUCHED = 0;
dword BuzzerTimer = 0;
Buzz_en BuzzState = BUZZ_OFF;
//PEN_State_en Pointer_State = PEN_IDLE;
//dword PEN_Debounce_Time = 0;
extern byte id;
extern dword Timer_id_CurrVal;


const PORT_CONF Port_configuration[] = { 
  {PORT_A, &PTAD, &PTADD, &PTAPE, 2,
    { {0,IO_DIR_INPUT,PULLUP_ENABLE,0}, //SW5  //Adult/Child
      {1,IO_DIR_INPUT,PULLUP_ENABLE,0}, //SW4  //Increment   
    }},
  {PORT_B, &PTBD, &PTBDD, &PTBPE, 3,//4,
    { {0,IO_DIR_INPUT,PULLUP_ENABLE,0}, //SW3  //RVG/Film   
      {1,IO_DIR_INPUT,PULLUP_DISABLE,0}, //VADC     
      {2,IO_DIR_INPUT,PULLUP_ENABLE,0}, //SW2  //Op mode(molar, premolar, incisor)(LEDs - Incisor[A+K5], Premolar[B+K5], Molar[H+K5])
      {3,IO_DIR_INPUT,PULLUP_ENABLE,0}, //SW1  //Decrement
    }},
  {PORT_C, &PTCD, &PTCDD, &PTCPE, 6,
    { 
      {0,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //K5
      {1,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //START
      {2,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //LDE1
      {3,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //ERR
      {4,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //PST
      {5,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //EXP
      
    }},
  {PORT_D, &PTDD, &PTDDD, &PTDPE, 4,
    { {0,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //BUZZ1    
      {1,IO_DIR_INPUT,PULLUP_DISABLE,0}, //AD1 
      {2,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //XPOSE1
      {3,IO_DIR_INPUT,PULLUP_DISABLE,0}, //TCON#
    }},
  {PORT_E, &PTED, &PTEDD, &PTEPE, 8,
    { {0,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //A
      {1,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //B
      {2,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //C   //RVG - C and K5
      {3,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //D    .
      {4,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //E   
      {5,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //F
      {6,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //G
      {7,IO_DIR_OUTPUT,PULLUP_DISABLE,0}, //H
    }},
  {PORT_F, &PTFD, &PTFDD, &PTFPE, 4,
    { 
      {0,IO_DIR_OUTPUT,PULLUP_DISABLE,0},//K4      
      {1,IO_DIR_OUTPUT,PULLUP_DISABLE,0},//K3      
      {4,IO_DIR_OUTPUT,PULLUP_DISABLE,0},//K2      
      {5,IO_DIR_OUTPUT,PULLUP_DISABLE,0},//K1      
    }},
  {PORT_G, &PTGD, &PTGDD, &PTGPE, 4,
    { {0,IO_DIR_INPUT,PULLUP_ENABLE,0}, //SW8   //Upper jaw/Lower jaw   (LEDs - )
      {1,IO_DIR_INPUT,PULLUP_ENABLE,0}, //SW7   //Set
      {2,IO_DIR_INPUT,PULLUP_ENABLE,0}, //SW6   //Start/Stop
      {3,IO_DIR_INPUT,PULLUP_DISABLE,0},//ZDTC      
    }},
};

const word LED_List[LED_MAX] = {
  LED_D1,
  LED_D2,
  LED_D3,
  LED_D4,
  LED_D5,
  LED_D6,
  LED_D7,
  LED_D8,
  LED_D9,
  LED_D10,
  LED_D11,
  LED_D12,
  LED_D13,
  
};



void InitPortPins() {
  byte channel_index = 0;
  byte port_index = 0;
  byte bit_mask = 0x01;
  
  //Reset I2C enable bit        
  //IIC1C &= 0x7f;
  
  for(port_index = 0; port_index < PORT_COUNT; port_index++) {
    for(channel_index = 0; channel_index < Port_configuration[port_index].UsedPinCount; channel_index++) {
      bit_mask = 0x01 << Port_configuration[port_index].Pins[channel_index].Pin_num;
      //Direction
      if(Port_configuration[port_index].Pins[channel_index].Direction == IO_DIR_INPUT) {
          *Port_configuration[port_index].Port_Mode &= (~bit_mask);
      } else {          
          *Port_configuration[port_index].Port_Mode |= bit_mask;
      }
      
      //Pullup
      if(Port_configuration[port_index].Pins[channel_index].Pull_Up_En == PULLUP_ENABLE) {
          *Port_configuration[port_index].Port_Pullup |= bit_mask;
      } else {
          *Port_configuration[port_index].Port_Pullup &= (~bit_mask);
      }
      
      //Default value
      if(Port_configuration[port_index].Pins[channel_index].Direction == IO_DIR_OUTPUT) {        
      
        if(Port_configuration[port_index].Pins[channel_index].Default_val == 1) {
          *Port_configuration[port_index].HW_Port |= bit_mask;
        } else {
          *Port_configuration[port_index].HW_Port &= (~bit_mask);
        }
        
      }
      
    }
  }
}


//Function to change buzzer status
void SetBuzzer() {

  switch(BuzzState) {
    case BUZZ_OFF:
    PTDD &= 0xfe;//PD0
    BuzzerTimer = 0;
    break;
    
    case BUZZ_TIMED_OFF:
    if(BuzzerTimer == 0) {
      BuzzerTimer = BUZZER_ON_TIME;
      BuzzState = BUZZ_TIMED_ON;      
      PTDD |= 0x01;//PD0_bit0
      //Blimk the LED      
      ChangeSETLed(1); 
    }
    break;
    
    case BUZZ_TIMED_ON:
    if(BuzzerTimer == 0) {
      BuzzerTimer = BUZZER_OFF_TIME;
      BuzzState = BUZZ_TIMED_OFF;      
      PTDD &= 0xfe;//PD0_bit0
      ChangeSETLed(0); 
    }
    break;
    
    case BUZZ_CONTINUOUS:
    PTDD |= 0x01;//PD0_bit0
    BuzzerTimer = 0;
    break;
    
  }
  
}
/*
//Poll for PENIRQ signal
void PollPENIRQ() {
  
  switch(Pointer_State) {
    case PEN_IDLE:
    //Start counting for debounce of touch event
      if(!(PTED & 0x10)) {
        PEN_Debounce_Time = PEN_DEBOUNCE_TIME;
        Pointer_State = PEN_ACTIVE;
      } 
    break;
    case PEN_ACTIVE:
    //Check if debounce timer elapsed.
      if(PEN_Debounce_Time == 0) {
        Pointer_State = PEN_DEBOUNCING;
      }
    break;
    case PEN_DEBOUNCING:
      //check if the PEN_IRQ is still pressed.
      if(!(PTED & 0x10)) {
        Pointer_State = PEN_DETECTED;
      } else {
        Pointer_State = PEN_IDLE;
      }
    break;
    case PEN_DETECTED:
      PollTouchPosition();
      if((PTED & 0x10)) {
         //Pointer_State = PEN_IDLE;
      }
    break;
    
  }
}
*/
//Interface updates the LED displayed.
void UpdateLED(word config) {
  byte indx = 0;
  byte Led_Mux = 0;  
  byte Led_States = 0;
  word temp = config & 0x00ff;
  
  Led_Mux = (byte)temp;//config & 0xff;
  Led_States = (config >> 8) & 0xff;
  /*if(config != 0) {
    for(indx = 0;indx < LED_MAX;indx++) {
      if(config & LED_List[indx]) {      
        if(indx < 8) {        
          Led_Mux |= LED_List[indx];          
        }else {
        //PC1,PC2,PC3,PC4,PC5 
          Led_States |= LED_List[indx] >> 8;
        }
      }
      __RESET_WATCHDOG(); 
    }
  }*/
  
  //Set the mux to select LEDs
  LED_SEGMENT_MUX &= LED_SEG_MUX_RESET;
  LED_STATES &= LED_STATES_RESET;
  
  SEVEN_SEG_SELECT &= SEVEN_SEG_RESET;
  
  LED_SEG_SELECT = 1;
  
  __RESET_WATCHDOG();
  
  //Mask the values
  //Led_Mux = Led_Mux & (~LED_SEG_MUX_RESET);  
  Led_States = (Led_States<<1) & (~LED_STATES_RESET);  
  
  
  LED_SEGMENT_MUX = Led_Mux;  
  LED_STATES |= Led_States;
}



