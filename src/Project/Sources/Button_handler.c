#include <MC9S08AW16.h>
#include "Button_handler.h"

const tst_ButtonEvent Button_Events[EVENT_MAX] = 
{
  {BUTTON_OPMODE, EVENT_OPMODE, 0},//EVENT_OPMODE,  
  {BUTTON_JAW, EVENT_JAW, 1},//EVENT_JAW,
  {BUTTON_ADLTCHLD, EVENT_ADULTCHILD, 2},//EVENT_ADULTCHILD,
  {BUTTON_PROGRAM, EVENT_PROGRAM, 3},//EVENT_PROGRAM,
  {BUTTON_SET, EVENT_SET, 4},//EVENT_SET,
  {BUTTON_RVGFILM, EVENT_RVGFILM, 5},//EVENT_RVGFILM,
  {BUTTON_INCREMENT, EVENT_INCREMENT, 6},//EVENT_INCREMENT,
  {BUTTON_DECREMENT, EVENT_DECREMENT, 7},//EVENT_DECREMENT,
};

const tst_LED_Map LED_Map[LED_ENUM_MAX] = 
{
  {LED_INCISOR, 0x0001},
  {LED_PREM, 0x0002},
  {LED_RVG, 0x0004},
  {LED_FILM, 0x0008},
  {LED_CHILD, 0x0010},
  {LED_JAW_DWN, 0x0020},
  {LED_ADULT, 0x0040},
  {LED_MOLAR, 0x0080},  
  {LED_START, 0x0100},  
  {LED_JAW_UP, 0x0200},
  {LED_LDE1, 0x0400},    
  {LED_PST, 0x0800},
  {LED_EXP, 0x1000},
  
};

byte ButtonBuffer[MAX_BUTTON_SAMPLES];
byte ScanSample = 0;
ButtonEvent_en CurrentButtonEvent = EVENT_NO_ACTIVITY;


//SW6 SW7 SW8 SW4 SW5 SW1 SW2 SW3
//Change the sequence to 
//SW1 SW2 SW3 SW4 SW5 SW6 SW7 SW8
//Function reads the status of button connected pins in to 
//selected sample buffer and increments the sample index.
void GetInputs()
{
  ButtonBuffer[ScanSample] = 0;
  //Get switch states  
  //SW1=PB3(Decrement), SW2=PB2(Opmode), SW3=PB0(RVG/Film)
  ButtonBuffer[ScanSample] |= PTBD_PTBD0;//(0x01 & PTBD);
  ButtonBuffer[ScanSample] |= (PTBD_PTBD3 << 2);//((0x0C & PTBD) >> 1);//SW1 Decrement
  ButtonBuffer[ScanSample] |= (PTBD_PTBD2 << 1);//((0x0C & PTBD) >> 1);//SW2 Opmode
  ButtonBuffer[ScanSample] = ButtonBuffer[ScanSample] << 2;
  
  //SW4=PA1(Increment), SW5=PA0(Adult/Child)
  ButtonBuffer[ScanSample] |= (0x03 & PTAD);
  ButtonBuffer[ScanSample] = ButtonBuffer[ScanSample] << 3;
  
  //SW6=PG2(Start), SW7=PG1(Set), SW8=PG0(Jaw selection)
  ButtonBuffer[ScanSample] |= ((0x07 & PTGD));
  
  ScanSample = (ScanSample + 1)%DEBOUNCED_SAMPLE;
}


//Function debounces the given button index with its samples
//in memory and updates the debounced sample.
void DebounceButtonIndex(byte butt_index)
{
  byte temp_res = 0x01 << butt_index;
  byte bitmask = temp_res;
  if(ButtonBuffer[DEBOUNCED_SAMPLE] & temp_res)
  {
    //Debounce for press
    temp_res &= (ButtonBuffer[SAMPLE_1] | ButtonBuffer[SAMPLE_2] | ButtonBuffer[SAMPLE_3]);
    if(!temp_res)
    {
      ButtonBuffer[DEBOUNCED_SAMPLE] &= (~bitmask);
    }
    
  } else
  {
    //Debounce for release
    temp_res &= (ButtonBuffer[SAMPLE_1] & ButtonBuffer[SAMPLE_2] & ButtonBuffer[SAMPLE_3]);
    if(temp_res)
    {
      ButtonBuffer[DEBOUNCED_SAMPLE] |= temp_res;
    }
  }
}

//Function to debounce the status of all buttons.
void DebounceButtons()
{
  byte ind;
  for(ind = 0;ind<BUTTON_MAX; ind++)
  {
    DebounceButtonIndex(ind);
  }
}

//Function to check for button pin status and debounce it on the run.
//Reads one sample for all keys in one call.
void ButtonJob()
{
  GetInputs();
  DebounceButtons();
  EvaluateButtons();
  
}

//Button manager to generate events on button press and release.
//To handle long press of buttons.

Button_Keys GetPressedButton()
{
  byte indx;
  Button_Keys ret_val = BUTTON_MAX;
  for(indx = 0;indx < BUTTON_MAX;indx++)
  {
    if(!(ButtonBuffer[DEBOUNCED_SAMPLE] & (0x01 << indx)))
    {
      ret_val = indx;
      break;
    }
  }
  return ret_val;
}

//Function checks for pressed keys and evaluates its corresponding event.
//Updates the event in the global variable.
void EvaluateButtons()
{
  byte index;
  Button_Keys temp_button;
  CurrentButtonEvent = EVENT_NO_ACTIVITY;
  temp_button = GetPressedButton();
  if(temp_button < BUTTON_MAX)
  {    
    for(index = 0;index < EVENT_MAX;index++)
    {
      if(Button_Events[index].Button == temp_button)
      {
        CurrentButtonEvent =  Button_Events[index].Event;
        break;
      }
    }
  }
}

//Function returns the last evaluated button event.
ButtonEvent_en GetCurrentButtonEvent()
{
  return CurrentButtonEvent;
}


//Function to initialize button states
void InitButtons()
{
  GetInputs();
  GetInputs();
  GetInputs();
}