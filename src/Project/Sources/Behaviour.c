#include <MC9S08AW16.h>
#include "IO_HWL.h"
#include "ADC_handler.h"
#include "Button_handler.h"
#include "Behaviour.h"
#include "InitRoutine.h"

extern Buzz_en BuzzState;
//extern const PosWindow_st KeyBoardLimits[KEY_MAX];
extern tst_ButtonEvent Button_Events[EVENT_MAX];
extern const byte SetBlinkTimer;
extern dword Timer_id_CurrVal;
extern const byte LongPressTimerId;
extern const byte XPOS_PauseTimerId;

extern InitRoutType_en InitType;
extern byte Segment_Override_Data;
extern byte Char_Override_Select;

extern JawSelection_en PrevJawSelection;
extern RVG_Film_en PrevRVGFilmState;
extern Adult_Child_en PrevAdultChildState;
extern SelectedVal_en PrevSelection;
extern word prev_PauseTime;



ControlState_en Behaviour_state;
RVG_Film_en RVGFilmState;
Adult_Child_en AdultChildState;
//tst_TouchEvent_st TouchEvent = {EVENT_NO_ACTIVITY,{0,0}};;
ButtonEvent_en ButtonEvent = EVENT_NO_ACTIVITY;
word DisplayCount;
word PrevDisplayCount;
MemConfig_st * ptr_MemConfg;
SelectedVal_en Selection;
JawSelection_en JawSelection;
XPOS_State_en XposeState; 
word LED_Config = 0;
//KeyEnable_en KeyState = STATE_IDLE;
//dword KeyStateTimer = 0;
word ValueStepCounter = 0;
HMI_State_en ValueEditState = HMI_IDLE;//If 1 value is incrementing, if 2 value is decrementing, if 0 idle
byte id = 0;
VoltageStatus_en VadcState;
byte DisplayBuffer[4];
byte DotBuffer = 0;
word MAX_DISPLAY_VAL = MAX_POSSIBLE_DISPLAY_VAL;
byte ErrorFlag = 0;
byte FATAL_VERROR = 0;
//byte XPOS_Count = 0;


  //    A  
  //   ###
  //  #   #
  // F#   #B
  //  #   #
  //   ###
  //  # G #
  // E#   #C
  //  #   #   
  //   ###   #H
  //    D
  /*                       //A,B,C,D,E,F,G,H
  const byte val0 = 0xfc;//0=1,1,1,1,1,1,0,0
  const byte val1 = 0x60;//1=0,1,1,0,0,0,0,0
  const byte val2 = 0xda;//2=1,1,0,1,1,0,1,0
  const byte val3 = 0xf2;//3=1,1,1,1,0,0,1,0
  const byte val4 = 0x66;//4=0,1,1,0,0,1,1,0
  const byte val5 = 0xb6;//5=1,0,1,1,0,1,1,0
  const byte val6 = 0xbe;//6=1,0,1,1,1,1,1,0
  const byte val7 = 0xe0;//7=1,1,1,0,0,0,0,0
  const byte val8 = 0xfe;//8=1,1,1,1,1,1,1,0
  const byte val9 = 0xf6;//9=1,1,1,1,0,1,1,0  */
  
  /*const byte DisplayData[10] = {
    0xfc,
    0x60,
    0xda,
    0xf2,
    0x66,
    0xb6,
    0xbe,
    0xe0,
    0xfe,
    0xf6
  };*/
  
  //    H  
  //   ###
  //  #   #
  // C#   #G
  //  #   #
  //   ###
  //  # B #
  // D#   #F
  //  #   #   
  //   ###   #A
  //    E
  /*                       //A,B,C,D,E,F,G,H
  const byte val0 = 0x3f;//0=0,0,1,1,1,1,1,1
  const byte val1 = 0x06;//1=0,0,0,0,0,1,1,0
  const byte val2 = 0x5b;//2=0,1,0,1,1,0,1,1
  const byte val3 = 0x4f;//3=0,1,0,0,1,1,1,1
  const byte val4 = 0x66;//4=0,1,1,0,0,1,1,0
  const byte val5 = 0x6d;//5=0,1,1,0,1,1,0,1
  const byte val6 = 0x7d;//6=0,1,1,1,1,1,0,1
  const byte val7 = 0x07;//7=0,0,0,0,0,1,1,1
  const byte val8 = 0x7f;//8=0,1,1,1,1,1,1,1
  const byte val9 = 0x6f;//9=0,1,1,0,1,1,1,1  
  const byte char_C = 0x39;//C=0,0,1,1,1,0,0,1
  const byte char_E = 0x79;//E=0,1,1,1,1,0,0,1
  const byte char_P = 0x73;//P=0,1,1,1,0,0,1,1
  const byte char_R = 0x50;//R=0,1,0,1,0,0,0,0*/
  
  
  //bit0 = A
  //bit1 = B
  //bit2 = C
  //bit3 = D
  //bit4 = E
  //bit5 = F
  //bit6 = G
  //bit7 = H
  //
  //    A  
  //   ###
  //  #   #
  // F#   #B
  //  #   #
  //   ###
  //  # G #
  // E#   #C
  //  #   #   
  //   ###   #H
  //    D
  
  const byte DisplayData[10] = {
    0x3f,
    0x06,
    0x5b,
    0x4f,
    0x66,
    0x6d,
    0x7d,
    0x07,
    0x7f,
    0x6f
  };
  
  const byte DisplayChar[27] = 
  {
    0,//A
    0,//B
    0x39,//C
    0,//D
    0x79,//E
    0,//F
    0,//G
    0,//H
    0,//I
    0,//J
    0,//K
    0,//L
    0,//M
    0,//N
    0,//O
    0x73,//P
    0,//Q
    0x50,//R
    0,//S
    0,//T
    0,//U
    0,//V
    0,//W
    0,//X
    0,//Y
    0,//Z
    0,//Invalid
  };
  
  //Assigning the flash address where the data has to be stored
  /*TimerConfig_st TimerConfig_val = { 
    {{0,(byte *)0xfc00,0,(byte *)0xfc02,0,(byte *)0xfc04,},{0,(byte *)0xfc06,0,(byte *)0xfc08,0,(byte *)0xfc0a,}}, 
    {{0,(byte *)0xfc0c,0,(byte *)0xfc0e,0,(byte *)0xfc10,},{0,(byte *)0xfc12,0,(byte *)0xfc14,0,(byte *)0xfc16,}},
    
  };*/
  
  JawConfig_st TimerConfig = 
  {
    { 
      {{0,0,0,(byte *)0xfc00,(byte *)0xfc02,(byte *)0xfc04,},{0,0,0,(byte *)0xfc06,(byte *)0xfc08,(byte *)0xfc0a,}}, 
      {{0,0,0,(byte *)0xfc0c,(byte *)0xfc0e,(byte *)0xfc10,},{0,0,0,(byte *)0xfc12,(byte *)0xfc14,(byte *)0xfc16,}},    
    },
    {
      {{0,0,0,(byte *)0xfc18,(byte *)0xfc1a,(byte *)0xfc1c,},{0,0,0,(byte *)0xfc1e,(byte *)0xfc20,(byte *)0xfc22,}}, 
      {{0,0,0,(byte *)0xfc24,(byte *)0xfc26,(byte *)0xfc28,},{0,0,0,(byte *)0xfc2a,(byte *)0xfc2c,(byte *)0xfc2e,}},    
    },
    0,(byte *)0xfc30,
  };
  //Assign the flash memory address where the Selection value has to be stored.
  byte* const AdultChildAddress = (byte*)0xfc32;
  byte* const RVGFilmAddress = (byte*)0xfc33;
  byte* const SelectionAddress = (byte*)0xfc34;
  byte* const JawSelectionAddress = (byte*)0xfc35;

//Initialize all behaviour related variables
void InitBehaviour() {
  Behaviour_state = IDLE;
  RVGFilmState = FILM;
  AdultChildState = ADULT;
  ButtonEvent = EVENT_NO_ACTIVITY;
  //TouchEvent.Touch_Position.X_value = 0;
  //TouchEvent.Touch_Position.Y_value = 0;
  DisplayCount = 0;
  PrevDisplayCount = 0;
  //Assign the pointer of default configuration for accessing memory
  ptr_MemConfg = &TimerConfig.JawUpperConfig.FilmConfig.AdultConfig;
  Selection = NO_SELECT;
  JawSelection = JAW_UPPER;
  //Read from FLASH memory
  //Ensure Little endian read
  TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Molar = WORD_VAL(TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Add_Molar);//*((word*)(TimerConfig.FilmConfig.AdultConfig.Add_Molar));
  if(TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Molar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Molar = 0;
  }
  TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.PreMolar = WORD_VAL(TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Add_PreMolar);
  if(TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.PreMolar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.PreMolar = 0;
  }
  TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Incisor = WORD_VAL(TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Add_Incisor);
  if(TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Incisor > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Incisor  = 0;
  }
  TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Molar = WORD_VAL(TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Add_Molar);
  if(TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Molar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Molar  = 0;
  }
  TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.PreMolar = WORD_VAL(TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Add_PreMolar);
  if(TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.PreMolar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.PreMolar = 0;
  }
  TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Incisor = WORD_VAL(TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Add_Incisor);
  if(TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Incisor > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Incisor = 0;
  }
  
  TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Molar = WORD_VAL(TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Add_Molar);
  if(TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Molar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Molar = 0;
  }
  TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.PreMolar = WORD_VAL(TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Add_PreMolar);
  if(TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.PreMolar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.PreMolar = 0;
  }
  TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Incisor = WORD_VAL(TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Add_Incisor);
  if(TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Incisor > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Incisor = 0;
  }
  TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Molar = WORD_VAL(TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Add_Molar);
  if(TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Molar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Molar = 0;
  }
  TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.PreMolar = WORD_VAL(TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Add_PreMolar);
  if(TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.PreMolar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.PreMolar = 0;
  }
  TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Incisor = WORD_VAL(TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Add_Incisor);
  if(TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Incisor > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Incisor = 0;
  }
  
  //Lower Jaw
  TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Molar = WORD_VAL(TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Add_Molar);//*((word*)(TimerConfig.FilmConfig.AdultConfig.Add_Molar));
  if(TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Molar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Molar = 0;
  }
  TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.PreMolar = WORD_VAL(TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Add_PreMolar);
  if(TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.PreMolar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.PreMolar = 0;
  }
  TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Incisor = WORD_VAL(TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Add_Incisor);
  if(TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Incisor > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Incisor  = 0;
  }
  TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Molar = WORD_VAL(TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Add_Molar);
  if(TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Molar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Molar  = 0;
  }
  TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.PreMolar = WORD_VAL(TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Add_PreMolar);
  if(TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.PreMolar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.PreMolar = 0;
  }
  TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Incisor = WORD_VAL(TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Add_Incisor);
  if(TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Incisor > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Incisor = 0;
  }
  
  TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Molar = WORD_VAL(TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Add_Molar);
  if(TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Molar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Molar = 0;
  }
  TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.PreMolar = WORD_VAL(TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Add_PreMolar);
  if(TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.PreMolar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.PreMolar = 0;
  }
  TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Incisor = WORD_VAL(TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Add_Incisor);
  if(TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Incisor > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Incisor = 0;
  }
  TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Molar = WORD_VAL(TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Add_Molar);
  if(TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Molar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Molar = 0;
  }
  TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.PreMolar = WORD_VAL(TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Add_PreMolar);
  if(TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.PreMolar > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.PreMolar = 0;
  }
  TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Incisor = WORD_VAL(TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Add_Incisor);
  if(TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Incisor > MAX_DISPLAY_VAL)
  {
    TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Incisor = 0;
  }
  
  TimerConfig.PauseTime = WORD_VAL(TimerConfig.Add_PauseTime);
  if(TimerConfig.PauseTime > MAX_PAUSE_VALUE)
  {
    TimerConfig.PauseTime = 0;
  }
  
  //Read Jaw selection
  JawSelection = (byte)(*JawSelectionAddress);
  PrevJawSelection = JawSelection;
  
  RVGFilmState = (byte)(*RVGFilmAddress);
  PrevRVGFilmState = RVGFilmState;
  
  AdultChildState = (byte)(*AdultChildAddress);
  PrevAdultChildState = AdultChildState;
  
  Selection = (byte)(*SelectionAddress);
  PrevSelection = Selection;
  
  prev_PauseTime = TimerConfig.PauseTime;
  
  ChangeSelectionStates();
  
  
  //Selection value initialization from Flash memory
  
  
    switch(Selection)
    {
      case MOLAR_SELECT:        
        DisplayCount = ptr_MemConfg->Molar;
        ChangeLEDState(LED_MOLAR,1);//Molar
        ChangeLEDState(LED_PREM,0);//PreMolar
        ChangeLEDState(LED_INCISOR,0);//Incisor
      break;
      case PREMOLAR_SELECT:
        DisplayCount = ptr_MemConfg->PreMolar;
        ChangeLEDState(LED_MOLAR,0);//Molar
        ChangeLEDState(LED_PREM,1);//PreMolar
        ChangeLEDState(LED_INCISOR,0);//Incisor
      break;
      case INCISOR_SELECT:        
        DisplayCount = ptr_MemConfg->Incisor;
        ChangeLEDState(LED_MOLAR,0);//Molar
        ChangeLEDState(LED_PREM,0);//PreMolar
        ChangeLEDState(LED_INCISOR,1);//Incisor
      break;
      default:
        Selection = MOLAR_SELECT;
        DisplayCount = ptr_MemConfg->Molar;
        ChangeLEDState(LED_MOLAR,1);//Molar
        ChangeLEDState(LED_PREM,0);//PreMolar
        ChangeLEDState(LED_INCISOR,0);//Incisor
      break;
    }
    
  
  
  
  //-------*/
  PrevDisplayCount = DisplayCount;
}


//Function to update the selection pointer based on the value of other
//parameters
void ChangeSelectionStates()
{
  switch(JawSelection)
  {
    case JAW_UPPER:
          //Read RVG/Film state
          ChangeLEDState(LED_JAW_UP, 1);
          ChangeLEDState(LED_JAW_DWN, 0);
        
        switch(RVGFilmState)
        {
          case FILM:
            RVGFilmState = FILM;
            //Assign the memory pointer of molar
            ptr_MemConfg = &TimerConfig.JawUpperConfig.FilmConfig.AdultConfig;      
            ChangeLEDState(LED_RVG,0);//RVG      
            ChangeLEDState(LED_FILM,1);//Film      
          break;
          case RVG:    
            RVGFilmState = RVG;
            //Assign the memory pointer of molar
            ptr_MemConfg = &TimerConfig.JawUpperConfig.RVGConfig.AdultConfig;      
            ChangeLEDState(LED_RVG,1);//RVG      
            ChangeLEDState(LED_FILM,0);//Film      
          break;
          default:
            //If invalid value, select the Film configuration
            RVGFilmState = FILM;
            //Assign the memory pointer of molar
            ptr_MemConfg = &TimerConfig.JawUpperConfig.FilmConfig.AdultConfig;      
            ChangeLEDState(LED_RVG,0);//RVG      
            ChangeLEDState(LED_FILM,1);//Film      
          break;
        }
        
        //Read Adult/Child state
        
        switch(AdultChildState)
        {
          case CHILD:
            if(RVGFilmState == FILM) {
              ptr_MemConfg = &TimerConfig.JawUpperConfig.FilmConfig.ChildConfig;
            } else
            {
              ptr_MemConfg = &TimerConfig.JawUpperConfig.RVGConfig.ChildConfig;
            }      
            ChangeLEDState(LED_ADULT,0);//Adult      
            ChangeLEDState(LED_CHILD,1);//Child      
          break;
          case ADULT:
            if(RVGFilmState == FILM) {
              ptr_MemConfg = &TimerConfig.JawUpperConfig.FilmConfig.AdultConfig;
            } else
            {
              ptr_MemConfg = &TimerConfig.JawUpperConfig.RVGConfig.AdultConfig;
            }      
            ChangeLEDState(LED_ADULT,1);//Adult      
            ChangeLEDState(LED_CHILD,0);//Child      
          break;
          default:
          //If value is not valid display adult config
            AdultChildState = ADULT;
            if(RVGFilmState == FILM) {
              ptr_MemConfg = &TimerConfig.JawUpperConfig.FilmConfig.AdultConfig;
            } else
            {
              ptr_MemConfg = &TimerConfig.JawUpperConfig.RVGConfig.AdultConfig;
            }      
            ChangeLEDState(LED_ADULT,1);//Adult      
            ChangeLEDState(LED_CHILD,0);//Child      
          break;
        }
    break;
    
    case JAW_LOWER:
    default:
              //Read RVG/Film state
          ChangeLEDState(LED_JAW_UP, 0);
          ChangeLEDState(LED_JAW_DWN, 1);
        
        switch(RVGFilmState)
        {
          case FILM:
            RVGFilmState = FILM;
            //Assign the memory pointer of molar
            ptr_MemConfg = &TimerConfig.JawLowerConfig.FilmConfig.AdultConfig;      
            ChangeLEDState(LED_RVG,0);//RVG      
            ChangeLEDState(LED_FILM,1);//Film      
          break;
          case RVG:    
            RVGFilmState = RVG;
            //Assign the memory pointer of molar
            ptr_MemConfg = &TimerConfig.JawLowerConfig.RVGConfig.AdultConfig;      
            ChangeLEDState(LED_RVG,1);//RVG      
            ChangeLEDState(LED_FILM,0);//Film      
          break;
          default:
            //If invalid value, select the Film configuration
            RVGFilmState = FILM;
            //Assign the memory pointer of molar
            ptr_MemConfg = &TimerConfig.JawLowerConfig.FilmConfig.AdultConfig;      
            ChangeLEDState(LED_RVG,0);//RVG      
            ChangeLEDState(LED_FILM,1);//Film      
          break;
        }
        
        //Read Adult/Child state
        
        switch(AdultChildState)
        {
          case CHILD:
            if(RVGFilmState == FILM) {
              ptr_MemConfg = &TimerConfig.JawLowerConfig.FilmConfig.ChildConfig;
            } else
            {
              ptr_MemConfg = &TimerConfig.JawLowerConfig.RVGConfig.ChildConfig;
            }      
            ChangeLEDState(LED_ADULT,0);//Adult      
            ChangeLEDState(LED_CHILD,1);//Child      
          break;
          case ADULT:
            if(RVGFilmState == FILM) {
              ptr_MemConfg = &TimerConfig.JawLowerConfig.FilmConfig.AdultConfig;
            } else
            {
              ptr_MemConfg = &TimerConfig.JawLowerConfig.RVGConfig.AdultConfig;
            }      
            ChangeLEDState(LED_ADULT,1);//Adult      
            ChangeLEDState(LED_CHILD,0);//Child      
          break;
          default:
          //If value is not valid display adult config
            AdultChildState = ADULT;
            if(RVGFilmState == FILM) {
              ptr_MemConfg = &TimerConfig.JawLowerConfig.FilmConfig.AdultConfig;
            } else
            {
              ptr_MemConfg = &TimerConfig.JawLowerConfig.RVGConfig.AdultConfig;
            }      
            ChangeLEDState(LED_ADULT,1);//Adult      
            ChangeLEDState(LED_CHILD,0);//Child      
          break;
        }

    break;
    
    
  }
}

//Restore the value of DisplayCount
void RestoreDispCount()
{
  
  DisplayCount = PrevDisplayCount;
  MAX_DISPLAY_VAL = MAX_POSSIBLE_DISPLAY_VAL;
  BuzzState = BUZZ_OFF;
}
//Job function for handling the behaviour
void BehaviourJob() {

//Test code----------
//DisplayCount = 1230;
//Selection = MOLAR_SELECT;

//-------------------
VoltageStatus_en adc_state = ADC_IDLE;

if(!FATAL_VERROR)
{
  
  
    __RESET_WATCHDOG();
    
    ButtonEvent = GetCurrentButtonEvent();
    
    
    //If currently XPose start is being active or about to be stopped.
    if((Behaviour_state == SET)||(Behaviour_state == STOP_SET)) {
    
      if(ButtonEvent == EVENT_SET)
      {
        id = Button_Events[BUTTON_SET].TimerID;
        GetSWTimer();
        //ChangeLEDState(LED_START,1);
        if(Timer_id_CurrVal == 0) {
          //Change Set state either set or cancel
          StopXPOSTimer();    
          ErrorFlag = 0;
          RestoreDispCount();          
        }
        Timer_id_CurrVal = KEY_STATE_TIMEOUT;
        StartThreadTimer(0,0);
      } else
      {      
        if(Behaviour_state != STOP_SET)
        { 
          //Trigger the X-Ray head connection ADC
          adc_state = IsHeadDisconnected();
          switch(XposeState) {
            case XPOS_IDLE:                                                    
            //Check if TCON is already is set
            //Set the error ERR3
            if(PTDD & 0x08)
            {
              ErrorFlag = 1;
              //StopXPOSTimer();
              DisplayBuffer[0] = 'E';
              DisplayBuffer[1] = 'R';
              DisplayBuffer[2] = 'R';
              DisplayBuffer[3] = '3';
            }
            else if((adc_state != ADC_CONVERTING)&&(adc_state == VOLTAGE_BELOW_LIMIT))
            { 
              ErrorFlag = 1;
              //StopXPOSTimer();
              DisplayBuffer[0] = 'E';
              DisplayBuffer[1] = 'R';
              DisplayBuffer[2] = 'R';
              DisplayBuffer[3] = '1';
            }
            else
            {
              ErrorFlag = 0;              
              //Timer will change the state to ON
              BuzzState = BUZZ_TIMED_OFF;
             
              XposeState = XPOS_TCON_WAIT;
            }            
            break;
            
            case XPOS_TCON_WAIT:           
            
            if((adc_state != ADC_CONVERTING)&&(adc_state == VOLTAGE_BELOW_LIMIT))
            {               
              ErrorFlag = 1;              
              DisplayBuffer[0] = 'E';
              DisplayBuffer[1] = 'R';
              DisplayBuffer[2] = 'R';
              DisplayBuffer[3] = '1';              
            }
            else if(PTDD & 0x08) {
            //Check if T-CON is enabled
            //PF4              
              XposeState = XPOS_ZDC_WAIT;              
              
            } else
            {              
              //Change the SET led status
              if((BuzzState != BUZZ_TIMED_OFF)&&(BuzzState != BUZZ_TIMED_ON))
              {             
                BuzzState = BUZZ_TIMED_OFF;
              }
            }
            break;          
            
            
            case XPOS_ZDC_WAIT:
            
            /*//TEST
            StartXPOSTimer(DisplayCount + TimerConfig.PauseTime);
              XposeState = XPOS_ON;
              ChangeSETLed(1);
              BuzzState = BUZZ_CONTINUOUS;
            //----*/
            
            if((adc_state != ADC_CONVERTING)&&(adc_state == VOLTAGE_BELOW_LIMIT))
            {               
              ErrorFlag = 1;              
              DisplayBuffer[0] = 'E';
              DisplayBuffer[1] = 'R';
              DisplayBuffer[2] = 'R';
              DisplayBuffer[3] = '1';
              
            }
            else if(PTGD & 0x08) {
              //Check for Zero cross detection            
              //Start XPOS timer              
              //Add pre heating time to xpose              
              StartXPOSTimer(DisplayCount + TimerConfig.PauseTime);
              XposeState = XPOS_ON;
              ChangeSETLed(1);
              ChangeLEDState(LED_EXP,1);//Expose            
              BuzzState = BUZZ_CONTINUOUS;              
            } 
            else
            {
              //do nothing
            }
            break;     
                              
            case XPOS_ON:
            
            
            
            //Check if TCON is lost
            if(!(PTDD & 0x08)) {
              //Change Set state either set or cancel
              ErrorFlag = 1;
              StopXPOSTimer();
              //Display error
              DisplayBuffer[0] = 'E';
              DisplayBuffer[1] = 'R';
              DisplayBuffer[2] = 'R';
              DisplayBuffer[3] = '2';              
            } else if((adc_state != ADC_CONVERTING)&&(adc_state == VOLTAGE_BELOW_LIMIT))
            {               
              ErrorFlag = 1;
              StopXPOSTimer();
              DisplayBuffer[0] = 'E';
              DisplayBuffer[1] = 'R';
              DisplayBuffer[2] = 'R';
              DisplayBuffer[3] = '1';
              
            }
             else
            {
              ErrorFlag = 0;
            }            
            
            break;
            
            case XPOS_IDLE_BEFORE_PAUSE:
              id = XPOS_PauseTimerId;
              GetSWTimer();
              if(Timer_id_CurrVal == 0)
              {                                       
                XposeState = XPOS_PAUSE_TIME;                
                Timer_id_CurrVal = XPOS_PAUSE_TIMEOUT;
                StartThreadTimer(0,0);
                DisplayCount = XPOS_PAUSE_TIMEOUT;      
              }
              ChangeSETLed(0);
            break;
                
            case XPOS_PAUSE_TIME:
              id = XPOS_PauseTimerId;
              GetSWTimer();
              
              if(Timer_id_CurrVal == 0) {
                
                RestoreDispCount();
                Behaviour_state = IDLE;
                XposeState = XPOS_IDLE;
            
              }
              ChangeSETLed(0);
            break;
            default:
            XposeState = XPOS_IDLE;
            
          }
        }
      }
      //Disable key if set is active
        ButtonEvent = EVENT_NO_ACTIVITY;
    }
    
    switch(ButtonEvent) {
      case EVENT_OPMODE:
      {
        id = Button_Events[BUTTON_OPMODE].TimerID;
        GetSWTimer();
        if(Timer_id_CurrVal == 0) {
          switch(Selection)
          {
            case INCISOR_SELECT:
              Behaviour_state = MOLAR;
              BuzzState = BUZZ_OFF;
              Selection = MOLAR_SELECT;
              DisplayCount = ptr_MemConfg->Molar;
              PrevDisplayCount = DisplayCount;
              MAX_DISPLAY_VAL = MAX_POSSIBLE_DISPLAY_VAL;
              ChangeLEDState(LED_MOLAR,1);//Molar
              ChangeLEDState(LED_PREM,0);//PreMolar
              ChangeLEDState(LED_INCISOR,0);//Incisor
              ErrorFlag = 0;
            break;
          
            case MOLAR_SELECT:
              Behaviour_state = PREMOLAR;
              BuzzState = BUZZ_OFF;
              Selection = PREMOLAR_SELECT;
              DisplayCount = ptr_MemConfg->PreMolar;
              PrevDisplayCount = DisplayCount;
              MAX_DISPLAY_VAL = MAX_POSSIBLE_DISPLAY_VAL;
              ChangeLEDState(LED_MOLAR,0);//Molar
              ChangeLEDState(LED_PREM,1);//PreMolar
              ChangeLEDState(LED_INCISOR,0);//Incisor
              ErrorFlag = 0;
            break;
            
            case PREMOLAR_SELECT:
              Behaviour_state = INCISOR;
              BuzzState = BUZZ_OFF;
              Selection = INCISOR_SELECT;
              DisplayCount = ptr_MemConfg->Incisor;
              PrevDisplayCount = DisplayCount;
              MAX_DISPLAY_VAL = MAX_POSSIBLE_DISPLAY_VAL;
              ChangeLEDState(LED_MOLAR,0);//Molar
              ChangeLEDState(LED_PREM,0);//PreMolar
              ChangeLEDState(LED_INCISOR,1);//Incisor
              ErrorFlag = 0;
            break;
            default:
            break;
          }
        }        
      }
      Timer_id_CurrVal = KEY_STATE_TIMEOUT;
      StartThreadTimer(0,0);
        
      break;     
      
      
      case EVENT_ADULTCHILD:
        ErrorFlag = 0;
        id = Button_Events[BUTTON_ADLTCHLD].TimerID;
        GetSWTimer();
        if(Timer_id_CurrVal == 0) {
          
          if(AdultChildState == ADULT) {
            AdultChildState = CHILD;
            //Assign the memory pointer of molar
            ChangeSelectionStates();
            
            UpdateDispCount();
            ChangeLEDState(LED_ADULT,0);//Adult      
            ChangeLEDState(LED_CHILD,1);//Child      
          }else{
            AdultChildState = ADULT;
            //Assign the memory pointer of molar
            ChangeSelectionStates();
            
            UpdateDispCount();
            ChangeLEDState(LED_ADULT,1);//Adult      
            ChangeLEDState(LED_CHILD,0);//Child      
          }
        }
        Timer_id_CurrVal = KEY_STATE_TIMEOUT;
        StartThreadTimer(0,0);
        
      break;
      
      case EVENT_PROGRAM:
      
        if((Behaviour_state != PAUSE_TIME_EDIT)&&(Behaviour_state != PAUSE_TIME_CONFIRM))
        {          
          id = LongPressTimerId;
          GetSWTimer();
          if(Timer_id_CurrVal == 0)
          {
            if((Behaviour_state != PROGRAM_WAIT_LONGPRESS)&&(Behaviour_state != PAUSE_TIME))
            {   
              
              Behaviour_state = PROGRAM_WAIT_LONGPRESS;
              id = LongPressTimerId;
              Timer_id_CurrVal = KEY_SET_LONGPRESS;
              StartThreadTimer(0,0);
              
            } else
            {
              //When Long press is detected
              //Change the screen to Pause time setting
              Behaviour_state = PAUSE_TIME;
              DisplayCount = TimerConfig.PauseTime;
              MAX_DISPLAY_VAL = MAX_PAUSE_VALUE;
              ChangeLEDState(LED_PST,1);
            }
          }
        } else
        {
          Behaviour_state = PAUSE_TIME_CONFIRM;
          ChangeLEDState(LED_PST,0);
        }
        id = Button_Events[BUTTON_PROGRAM].TimerID;
        Timer_id_CurrVal = KEY_STATE_TIMEOUT;
        StartThreadTimer(0,0);
        //ChangeLEDState(KEY_PROGRAM,1);
        break;
      
      case EVENT_SET:
        
        if(Behaviour_state != STOP_SET)
        { 
          Behaviour_state = SET_PRESS;
          BuzzState = BUZZ_OFF;
        }                  
                
        RestoreDispCount();
        
        //Every time press is detected, start the timer
        id = Button_Events[BUTTON_SET].TimerID;
        Timer_id_CurrVal = KEY_STATE_TIMEOUT;
        StartThreadTimer(0,0);
        //ChangeLEDState(KEY_SET,1);
        
        //If the value is 0, no need to start the counter
        if(DisplayCount == 0)
        {
          Behaviour_state = IDLE;
        }
        
      break;
      
      case EVENT_RVGFILM:
        ErrorFlag = 0;
        id = Button_Events[BUTTON_RVGFILM].TimerID;
        GetSWTimer();
        if(Timer_id_CurrVal == 0) {
          if(RVGFilmState == FILM) {
            RVGFilmState = RVG;
            //Assign the memory pointer of molar
            ChangeSelectionStates();
            
            UpdateDispCount();
            ChangeLEDState(LED_RVG,1);//RVG
            ChangeLEDState(LED_FILM,0);//FILM
          } else {
            RVGFilmState = FILM;
            //Assign the memory pointer of molar
            ChangeSelectionStates();
            
            UpdateDispCount();
            ChangeLEDState(LED_RVG,0);//RVG
            ChangeLEDState(LED_FILM,1);//FILM
          }
        }
        Timer_id_CurrVal = KEY_STATE_TIMEOUT;
        StartThreadTimer(0,0);
      break;
      
      case EVENT_INCREMENT:
        ValueEditState = HMI_INCREMENT;
        id = Button_Events[BUTTON_INCREMENT].TimerID;
        GetSWTimer();
        if(Timer_id_CurrVal == 0)
        {
          
          if(DisplayCount < MAX_DISPLAY_VAL) {
            
              if(ValueStepCounter < INCR_STEP1_LIMIT) {
                
                if((DisplayCount + 1) <= MAX_DISPLAY_VAL) {
                      
                  DisplayCount++;            
                }
              } else if(ValueStepCounter < INCR_STEP10_LIMIT) {
                if((DisplayCount + 10) <= MAX_DISPLAY_VAL) {
                  DisplayCount = DisplayCount + 10;
                } else {
                  DisplayCount = MAX_DISPLAY_VAL;
                }
              } else if(ValueStepCounter >= INCR_STEP10_LIMIT) {//ValueStepCounter < INCR_STEP100_LIMIT) {
                if((DisplayCount + 100) <= MAX_DISPLAY_VAL) {
                  DisplayCount = DisplayCount + 100;
                } else {
                  DisplayCount = MAX_DISPLAY_VAL;
                }
              } 
              /*else if(ValueStepCounter >= INCR_STEP100_LIMIT) {
                if((DisplayCount + 1000) <= MAX_DISPLAY_VAL) {
                  DisplayCount = DisplayCount + 1000;
                } else {
                  DisplayCount = MAX_DISPLAY_VAL;
                }
              }*/ 
              
              
          }   
            if(ValueStepCounter < INCR_STEP100_LIMIT) {
              
            ValueStepCounter++;
            }   
          
          
            Timer_id_CurrVal = KEY_STATE_TIMEOUT;
            StartThreadTimer(0,0);
            
            //ChangeLEDState(KEY_INCREMENT,1);
            //If pause time is edited, the previous edited value of expose time should be
            //loaded back after the pause time edit is completed.
            if(Behaviour_state != PAUSE_TIME_EDIT)
            {
              PrevDisplayCount = DisplayCount;
            }
        }
        
      break;
      
      case EVENT_DECREMENT:
        ValueEditState = HMI_DECREMENT;
        id = Button_Events[BUTTON_DECREMENT].TimerID;
        GetSWTimer();
        if(Timer_id_CurrVal == 0)
        {
          if(DisplayCount > 0) {
            if(ValueStepCounter < INCR_STEP1_LIMIT) {
              
              DisplayCount--;
            } else if(ValueStepCounter < INCR_STEP10_LIMIT) {
              if((DisplayCount - 10) <= MAX_DISPLAY_VAL) {
                DisplayCount = DisplayCount - 10;
              } else {
                DisplayCount = 0;
              }
            } else if(ValueStepCounter >= INCR_STEP10_LIMIT) {//ValueStepCounter < INCR_STEP100_LIMIT) {
              if((DisplayCount - 100) <= MAX_DISPLAY_VAL) {
                DisplayCount = DisplayCount - 100;
              } else {
                DisplayCount = 0;
              }
            }  
            /*else if(ValueStepCounter >= INCR_STEP100_LIMIT) {
              if((DisplayCount - 1000) <= MAX_DISPLAY_VAL) {
                DisplayCount = DisplayCount - 1000;
              } else {
                DisplayCount = 0;
              }
            } */
          }
          if(ValueStepCounter < INCR_STEP100_LIMIT) {
            
            ValueStepCounter++;
          }
          id = Button_Events[BUTTON_DECREMENT].TimerID;
          Timer_id_CurrVal = KEY_STATE_TIMEOUT;
          StartThreadTimer(0,0);
          //ChangeLEDState(KEY_DECREMENT,1);
          if(Behaviour_state != PAUSE_TIME_EDIT)
          {
            PrevDisplayCount = DisplayCount;
          }
        }
      break;
      
      case EVENT_JAW:
      ErrorFlag = 0;
        id = Button_Events[BUTTON_JAW].TimerID;
        GetSWTimer();
        if(Timer_id_CurrVal == 0) {
          if(JawSelection == JAW_UPPER) {
            JawSelection = JAW_LOWER;
            //Assign the memory pointer of molar
            ChangeSelectionStates();
            
            UpdateDispCount();
            ChangeLEDState(LED_JAW_DWN,1);//JAW_DOWN
            ChangeLEDState(LED_JAW_UP,0);//JAW_UP
          } else {
            JawSelection = JAW_UPPER;
            //Assign the memory pointer of molar
            ChangeSelectionStates();
            
            UpdateDispCount();
            ChangeLEDState(LED_JAW_DWN,0);//JAW_DOWN
            ChangeLEDState(LED_JAW_UP,1);//JAW_UP
          }
        }
        Timer_id_CurrVal = KEY_STATE_TIMEOUT;
        StartThreadTimer(0,0);
      break;
      
      case EVENT_NO_ACTIVITY:      
      //Reset the leds when idle
      ResetKeyStates();
      break;
      
      default:
      
      break;
    }   
    
  }
  
  //Reset Pause time led, if behaviour state is not editing pause time
  if((Behaviour_state != PAUSE_TIME)&&(Behaviour_state != PAUSE_TIME_EDIT))
  {
    ChangeLEDState(LED_PST,0);
  }
  UpdateDisplayString();
}

//Function to convert value into string or update display.
void UpdateDisplayString()
{
  //Update DisplayBuffer with characters to be displayed.
  word temp_val = DisplayCount;
  byte indx = 0; 
  
  
  DotBuffer = 0;
  
  //Check if the error has occured
  VadcState = IsVoltageHigh();
  if(VadcState == VOLTAGE_EXCEED_LIMIT) {
    //Set PC3, error led
    //PTCD |= 0x08;
    ChangeLEDState(LED_LDE1,1);//Error
    DisplayBuffer[0] = 'E';
    DisplayBuffer[1] = 'R';
    DisplayBuffer[2] = 'R';
    DisplayBuffer[3] = '5';
    FATAL_VERROR = 1;
    
  } else if(VadcState == VOLTAGE_BELOW_LIMIT){
    //Set PC3, error led
    //PTCD |= 0x08;
    ChangeLEDState(LED_LDE1,1);//Error
    DisplayBuffer[0] = 'E';
    DisplayBuffer[1] = 'R';
    DisplayBuffer[2] = 'R';
    DisplayBuffer[3] = '4';
    FATAL_VERROR = 1;
    
  } else
  {
    FATAL_VERROR = 0;
    if(!ErrorFlag)
    {
      
      //PTCD &= 0xf7;
      ChangeLEDState(LED_LDE1,0);//Error
      if(temp_val > 0)
      {      
        for(indx = 0;indx < 4;indx++)  
        {
          if((temp_val % 10) || (temp_val > 0))
          {
            DisplayBuffer[3 - indx] = '0'+(temp_val % 10);
            temp_val = temp_val / 10;
          } else
          {
            DisplayBuffer[3 - indx] = 0;
          }
        }
        
      } else
      {
        //Reset the buffer
        DisplayBuffer[0] = 0;
        DisplayBuffer[1] = 0;
        DisplayBuffer[2] = 0;
        if(InitType == INVALID)
        {          
          DisplayBuffer[3] = '0';
        } else
        {
          DisplayBuffer[3] = 0;
        }
      }
      //If editing Pause time
      if((Behaviour_state == PAUSE_TIME_EDIT)||(Behaviour_state == PAUSE_TIME))
      {
        DisplayBuffer[0] = 'P';
      }
      if(XposeState == XPOS_PAUSE_TIME)
      {
        DisplayBuffer[0] = 'C';
        DotBuffer |= CHAR4_DOT_ENABLE;
      }
    } else
    {
      //Set PE2, error led
      //PTCD |= 0x08;
      ChangeLEDState(LED_LDE1,1);//Error
    }
  }
}

//Reset the key state LEDs
void ResetKeyStates() {

      id = Button_Events[BUTTON_INCREMENT].TimerID;
      GetSWTimer();
      if(Timer_id_CurrVal == 0) {
        if((ValueEditState == HMI_INCREMENT)||(ValueEditState == HMI_IDLE)) {
          
          ValueStepCounter = 0;
        }
        //ChangeLEDState(KEY_INCREMENT,0);
      }
      
      id = Button_Events[BUTTON_DECREMENT].TimerID;
      GetSWTimer();
      if(Timer_id_CurrVal == 0) {
        if((ValueEditState == HMI_DECREMENT)||(ValueEditState == HMI_IDLE)) {
        
          ValueStepCounter = 0;
        }
        //ChangeLEDState(KEY_DECREMENT,0);
      }
      
      id = Button_Events[BUTTON_PROGRAM].TimerID;
      GetSWTimer();
      if(Timer_id_CurrVal == 0) {
        //ChangeLEDState(KEY_PROGRAM,0);
        id = LongPressTimerId;
        GetSWTimer();
        if(((Behaviour_state == PROGRAM_WAIT_LONGPRESS)&&(Timer_id_CurrVal != 0))||
          ((Behaviour_state == PAUSE_TIME_CONFIRM)&&(Timer_id_CurrVal == 0)))
        {          
          
          //Save the value to the selected configuration
          if(Behaviour_state == PAUSE_TIME_CONFIRM)
          {
            TimerConfig.PauseTime = DisplayCount;
            RestoreDispCount();
          } else
          {
            switch(Selection) {
              case MOLAR_SELECT:
                ptr_MemConfg->Molar = DisplayCount;
              break;
              case PREMOLAR_SELECT:
                ptr_MemConfg->PreMolar = DisplayCount;
              break;
              case INCISOR_SELECT:
                ptr_MemConfg->Incisor = DisplayCount;
              break;
              default:break;
              
            }
            PrevDisplayCount = DisplayCount;
          }
          Behaviour_state = PROGRAM_RELEASE;
          BuzzState = BUZZ_CONTINUOUS;
        } else if(Behaviour_state == PAUSE_TIME)
        {
          Behaviour_state = PAUSE_TIME_EDIT;
        }
      }
      
      id = Button_Events[BUTTON_SET].TimerID;
      GetSWTimer();
      if(Timer_id_CurrVal == 0) { 
                
        if(Behaviour_state == SET_PRESS)
        {
          Behaviour_state = SET;
        } else if(Behaviour_state == STOP_SET)
        {
          Behaviour_state = IDLE;
        }
      }
}

//Update the LED display for a given LED
void ChangeLEDState(LEDs_en led_indx, byte val) {
  //Reset other states. Not toggle leds
  dword rem_time = 0;  
  
  if(led_indx < LED_ENUM_MAX) {    
      
      if(val == 0) {
        LED_Config &= (~(LED_Map[led_indx].Bitmask));
      } else {
        LED_Config |= LED_Map[led_indx].Bitmask;
      }      
  }
}

//Update SET Led
void ChangeSETLed(byte val)
{  
    ChangeLEDState(LED_START,val);    
}

//Update the value of DisplayCount
void UpdateDispCount() {
  if(Selection == MOLAR) {
    DisplayCount = ptr_MemConfg->Molar;
    PrevDisplayCount = DisplayCount;
    MAX_DISPLAY_VAL = MAX_POSSIBLE_DISPLAY_VAL;
  } else if(Selection == PREMOLAR) {
    DisplayCount = ptr_MemConfg->PreMolar;
    PrevDisplayCount = DisplayCount;
    MAX_DISPLAY_VAL = MAX_POSSIBLE_DISPLAY_VAL;
  } else if(Selection == INCISOR) {
    DisplayCount = ptr_MemConfg->Incisor;
    PrevDisplayCount = DisplayCount;
    MAX_DISPLAY_VAL = MAX_POSSIBLE_DISPLAY_VAL;
  }
}

//Handle the display of segments
void DisplayHMI() {
  
  static byte index = 0;
  word temp_count = DisplayCount;
  byte temp_digit = 0;
  byte temp_digit2 = 0;
  byte temp_dot = 0;
  
  byte mask = 0x01;        
        int i = 0;
  
  //Segment pins - PE0-PE7  
      
      switch(index) {
        case 0:  
          
          temp_count = DisplayBuffer[3];
          temp_dot = DotBuffer & CHAR1_DOT_ENABLE;
          SEG_DISP_2 = 0;
          SEG_DISP_3 = 0;
          SEG_DISP_4 = 0;
          LED_STATES &= LED_STATES_RESET;
          //Reset the segments
          PTED &= 0x00;          
          SEG_DISP_1 = 1;
        break;
       case 1:  
          LED_SEG_SELECT = 0;
          temp_count = DisplayBuffer[2];
          temp_dot = DotBuffer & CHAR2_DOT_ENABLE;
          SEG_DISP_1 = 0;
          SEG_DISP_3 = 0;
          SEG_DISP_4 = 0;
          LED_STATES &= LED_STATES_RESET;
          //Reset the segments
          PTED &= 0x00;          
          SEG_DISP_2 = 1;          
        break;
        case 2:
          LED_SEG_SELECT = 0;          
          temp_count = DisplayBuffer[1];
          temp_dot = DotBuffer & CHAR3_DOT_ENABLE;
          SEG_DISP_1 = 0;
          SEG_DISP_2 = 0;
          SEG_DISP_4 = 0;
          LED_STATES &= LED_STATES_RESET;
          //Reset the segments
          PTED &= 0x00;          
          SEG_DISP_3 = 1;          
        break;
        case 3:          
          temp_count = DisplayBuffer[0];
          temp_dot = DotBuffer & CHAR4_DOT_ENABLE;
          LED_SEG_SELECT = 0;
          SEG_DISP_1 = 0;
          SEG_DISP_2 = 0;
          SEG_DISP_3 = 0;
          LED_STATES &= LED_STATES_RESET;
          //Reset the segments
          PTED &= 0x00;          
          SEG_DISP_4 = 1;          
        break;
        case 4:
          __RESET_WATCHDOG();
          LED_SEG_SELECT = 0;
          SEG_DISP_1 = 0;
          SEG_DISP_2 = 0;
          SEG_DISP_3 = 0;
          SEG_DISP_4 = 0;
          //Reset the segments
          PTED &= 0x00;          
          UpdateLED(LED_Config);
          break;
        
      }  
      //Update the segment pins for segment LCDs
      if(index < 4) {        
        
        temp_digit = temp_count;
        
        if((temp_digit >= '0')&&(temp_digit <= '9'))
        {          
          temp_digit = DisplayData[temp_digit - (byte)('0')];
        } else if((temp_digit >= 'A')&&(temp_digit <= 'Z'))
        {
          temp_digit = DisplayChar[temp_digit - (byte)('A')];
        } else
        {
          temp_digit = DisplayChar[26];
        }
        
        if(temp_dot)
        {
          temp_digit |= 0x80;
        }        
        
        //For init routine
        if((InitType == SEVEN_SEG_ROLL)&&(index == Char_Override_Select))
        {
           temp_digit = Segment_Override_Data;
        }
                
        //Set the bits
        PTED = temp_digit;
        
        
        
        LED_SEG_SELECT = 0;       
      
      }
   
    
    index = (index + 1)%5;  
  
}  