

typedef enum
{
  SAMPLE_1,
  SAMPLE_2,
  SAMPLE_3,
  DEBOUNCED_SAMPLE,
  MAX_BUTTON_SAMPLES,
}SampleIndex_en;

//SW1 SW2 SW3 SW4 SW5 SW6 SW7 SW8
typedef enum
{
  BUTTON_JAW,
  BUTTON_PROGRAM,
  BUTTON_SET,
  BUTTON_ADLTCHLD,
  BUTTON_INCREMENT,
  BUTTON_RVGFILM,
  BUTTON_OPMODE,
  BUTTON_DECREMENT,
  
  BUTTON_MAX,
}Button_Keys;

//Button events after debouncing
typedef enum enum_ButtonEvent{
  
  //EVENT_PRESS,
  //EVENT_LONG_PRESS,  
  EVENT_OPMODE,  
  EVENT_JAW,
  EVENT_ADULTCHILD,
  EVENT_PROGRAM,
  EVENT_SET,
  EVENT_RVGFILM,
  EVENT_INCREMENT,
  EVENT_DECREMENT,
  
  EVENT_MAX,  
  EVENT_NO_ACTIVITY,
  
}ButtonEvent_en;

typedef enum enum_LEDs
{
  LED_INCISOR,//A
  LED_PREM,   //B
  LED_RVG,    //C
  LED_FILM,   //D
  LED_CHILD,  //E
  LED_JAW_DWN,//F
  LED_ADULT,  //G
  LED_MOLAR,  //H
  LED_START,  //Start
  LED_JAW_UP, //Err
  LED_LDE1,   //LDE1  
  LED_PST,    //PST
  LED_EXP,    //EXP 
  
    
  LED_ENUM_MAX
}LEDs_en;

typedef struct st_LED_Map
{
  LEDs_en LED;
  word Bitmask;
  
}tst_LED_Map;

typedef struct st_ButtonEvent
{
  Button_Keys Button;
  ButtonEvent_en Event;
  byte TimerID;
  
}tst_ButtonEvent;

extern const tst_LED_Map LED_Map[LED_ENUM_MAX];


extern void GetInputs();
extern void DebounceButtonIndex(byte butt_index);
extern void DebounceButtons();
extern void ButtonJob();
extern Button_Keys GetPressedButton();
extern void EvaluateButtons();
extern ButtonEvent_en GetCurrentButtonEvent();
extern void InitButtons();