#include <MC9S08AW16.h>
#include "Timer.h"
#include "InitRoutine.h"
#include "Button_handler.h"
#include "Behaviour.h"

extern byte id;
extern dword Timer_id_CurrVal;
extern word LED_Config;
extern byte DotBuffer;
extern word DisplayCount;

extern const byte InitRoutTimerId;

InitRoutType_en InitType = ALL_ON;
byte Segment_Override_Data = 0;
byte Char_Override_Select = 0;

void InitRoutine()
{  
  byte rout_index; 
  byte roll_index;
  id = InitRoutTimerId;
              
  for(rout_index = ALL_ON; rout_index < TOTAL_INIT_ROUTINES; rout_index++)
  {
    
    while(InitType != INVALID)
    {
      GetSWTimer();
      if(Timer_id_CurrVal == 0)
      {        
        switch(InitType)
        {
          case ALL_ON:
            DisplayCount = 8888;
            UpdateDisplayString();
            DotBuffer = 0x0f;
            LED_Config = 0xffff;
            Timer_id_CurrVal = INIT_ROUTINE_TIME_STEP;
            StartThreadTimer(0,0);
            InitType = SEVEN_SEG_ON;
          break;
          case SEVEN_SEG_ON:
            DisplayCount = 8888;
            UpdateDisplayString();
            DotBuffer = 0x0f;
            LED_Config = 0;
            Timer_id_CurrVal = INIT_ROUTINE_TIME_STEP;
            StartThreadTimer(0,0);
            InitType = LEDS_ON;
          break;
          case LEDS_ON:
            DisplayCount = 0;
            UpdateDisplayString();
            DotBuffer = 0;
            LED_Config = 0xffff;
            Timer_id_CurrVal = INIT_ROUTINE_TIME_STEP;
            StartThreadTimer(0,0);
            InitType = SEVEN_SEG_ROLL;
            roll_index = 0;
          break;
          case SEVEN_SEG_ROLL:
            DisplayCount = 0;
            LED_Config = 0;
            UpdateDisplayString();
            DotBuffer = 0;
            if(roll_index < MAX_ALL_SEG_ROLL_INDEX)
            {
              Segment_Override_Data = 0x01 << (roll_index % MAX_EACH_CHAR_SEG_ROLL_INDEX);
              Char_Override_Select = roll_index / MAX_EACH_CHAR_SEG_ROLL_INDEX;
              Timer_id_CurrVal = INIT_ROUTINE_ROLL_STEP;
              StartThreadTimer(0,0);
              roll_index++;
            } else
            {
              InitType = INVALID;//LED_ROLL;
              roll_index = 0;
            }
          
          break;
          case LED_ROLL:
            DisplayCount = 0;
            UpdateDisplayString();
            DotBuffer = 0;
            if(roll_index < MAX_LED_ROLL_INDEX)
            {
              LED_Config = 0x01 << roll_index;
              Timer_id_CurrVal = INIT_ROUTINE_ROLL_STEP;
              StartThreadTimer(0,0);
              roll_index++;
            } else
            {
              InitType = INVALID;
            }
          break;
          default:
          break;
        }
      }
    }
  }
}