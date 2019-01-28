#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include <MC9S08AW16.h>
#include "Utilities.h"
#include "IO_HWL.h"
#include "ADC_handler.h"
#include "Button_handler.h"
#include "Behaviour.h"
#include "Timer.h"
#include "DoOnStack.h"
#include "InitRoutine.h"


extern JawConfig_st TimerConfig;
extern ControlState_en Behaviour_state;
extern byte* const JawSelectionAddress;
extern byte* RVGFilmAddress;
extern byte* AdultChildAddress;
extern byte* SelectionAddress;
extern JawSelection_en JawSelection;
extern RVG_Film_en RVGFilmState;
extern Adult_Child_en AdultChildState;
extern SelectedVal_en Selection;
extern Buzz_en BuzzState;
extern InitRoutType_en InitType;

JawSelection_en PrevJawSelection;
RVG_Film_en PrevRVGFilmState;
Adult_Child_en PrevAdultChildState;
SelectedVal_en PrevSelection;
word prev_PauseTime;
byte FlashWriteIndex = 1;
byte FaultCounter = 0;

void main(void) {
//int cnt_test = 0;
byte * address = 0;
word data = 0;
byte byte_data = 0;
byte byte_data2 = 0;


  
  /* include your code here */
  
  //Disable watchdog
  SOPT &= 0x7f;  

  InitClock();
  
  EnableInterrupts; /* enable interrupts */
  Delay(1);  
  
  InitPortPins();
  PTBDS |= 0x0d;
  PTGDS |= 0x07;
  PTADS |= 0x03;
  
  Delay(1);
  Init_Timer();
  //InitErrorADC();
  
  
  
  StartTimer(DISPLAY_FREQ);

  PrevRVGFilmState = RVGFilmState;
  PrevAdultChildState = AdultChildState;
  PrevSelection = Selection;
  prev_PauseTime = TimerConfig.PauseTime;
  
  //Call initialization routine display
  InitRoutine();
  InitType = INVALID;
  
  InitButtons();  
  InitBehaviour();  
  
  for(;;) {
  
  ButtonJob();  
    
  BehaviourJob();
  
  
  if((Behaviour_state == PROGRAM_RELEASE)||(PrevRVGFilmState != RVGFilmState)||
  (PrevJawSelection != JawSelection)||(PrevAdultChildState != AdultChildState)||(PrevSelection != Selection)||(prev_PauseTime != TimerConfig.PauseTime))
  {    
    
    //Write all the flash values
    //1.Film configuration
    //1.1 Adult configuration
    //1.1.1 Molar
    switch(FlashWriteIndex)
    {
      
      case 1:
    address = TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Add_Molar;
    data = TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Molar;    
    
    //Erase first
    FlashErase(address);
    
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    
    //Check 
    data = WORD_VAL(TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Add_Molar);    
    if(data == TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Molar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 2:
    
    //1.1.2 PreMolar
    address = TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Add_PreMolar;
    data = TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.PreMolar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    //Check 
    data = WORD_VAL(TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Add_PreMolar);    
    if(data == TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.PreMolar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    
    break;
    
      case 3:
    //1.1.3 Incisor
    address = TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Add_Incisor;
    data = TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Incisor;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Add_Incisor);
    if(data == TimerConfig.JawUpperConfig.FilmConfig.AdultConfig.Incisor)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 4:
    //1.2 Child configuration
    //1.2.1 Molar
    address = TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Add_Molar;
    data = TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Molar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Add_Molar);
    if(data == TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Molar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 5:    
    //1.2.2 PreMolar
    address = TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Add_PreMolar;
    data = TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.PreMolar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Add_PreMolar);
    if(data == TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.PreMolar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 6:
    //1.2.3 Incisor
    address = TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Add_Incisor;
    data = TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Incisor;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Add_Incisor);
    if(data == TimerConfig.JawUpperConfig.FilmConfig.ChildConfig.Incisor)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 7:
    //2.RVG configuration
    //2.1 Adult configuration
    //2.1.1 Molar
    address = TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Add_Molar;
    data = TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Molar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Add_Molar);
    if(data == TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Molar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 8:
    //2.1.2 PreMolar
    address = TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Add_PreMolar;
    data = TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.PreMolar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Add_PreMolar);
    if(data == TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.PreMolar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 9:
    //2.1.3 Incisor
    address = TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Add_Incisor;
    data = TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Incisor;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Add_Incisor);
    if(data == TimerConfig.JawUpperConfig.RVGConfig.AdultConfig.Incisor)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 10:
    //2.2 Child configuration
    //2.2.1 Molar
    address = TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Add_Molar;
    data = TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Molar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Add_Molar);
    if(data == TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Molar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 11:
    //2.2.2 PreMolar
    address = TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Add_PreMolar;
    data = TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.PreMolar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Add_PreMolar);
    if(data == TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.PreMolar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 12:
    //2.2.3 Incisor
    address = TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Add_Incisor;
    data = TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Incisor;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Add_Incisor);
    if(data == TimerConfig.JawUpperConfig.RVGConfig.ChildConfig.Incisor)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
    //For JawLower configuration
    case 13:
    address = TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Add_Molar;
    data = TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Molar;    
    
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    
    //Check 
    data = WORD_VAL(TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Add_Molar);    
    if(data == TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Molar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 14:
    
    //1.1.2 PreMolar
    address = TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Add_PreMolar;
    data = TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.PreMolar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    //Check 
    data = WORD_VAL(TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Add_PreMolar);    
    if(data == TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.PreMolar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    
    break;
    
      case 15:
    //1.1.3 Incisor
    address = TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Add_Incisor;
    data = TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Incisor;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Add_Incisor);
    if(data == TimerConfig.JawLowerConfig.FilmConfig.AdultConfig.Incisor)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 16:
    //1.2 Child configuration
    //1.2.1 Molar
    address = TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Add_Molar;
    data = TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Molar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Add_Molar);
    if(data == TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Molar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 17:    
    //1.2.2 PreMolar
    address = TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Add_PreMolar;
    data = TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.PreMolar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Add_PreMolar);
    if(data == TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.PreMolar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 18:
    //1.2.3 Incisor
    address = TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Add_Incisor;
    data = TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Incisor;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Add_Incisor);
    if(data == TimerConfig.JawLowerConfig.FilmConfig.ChildConfig.Incisor)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 19:
    //2.RVG configuration
    //2.1 Adult configuration
    //2.1.1 Molar
    address = TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Add_Molar;
    data = TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Molar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Add_Molar);
    if(data == TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Molar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 20:
    //2.1.2 PreMolar
    address = TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Add_PreMolar;
    data = TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.PreMolar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Add_PreMolar);
    if(data == TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.PreMolar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 21:
    //2.1.3 Incisor
    address = TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Add_Incisor;
    data = TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Incisor;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Add_Incisor);
    if(data == TimerConfig.JawLowerConfig.RVGConfig.AdultConfig.Incisor)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 22:
    //2.2 Child configuration
    //2.2.1 Molar
    address = TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Add_Molar;
    data = TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Molar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Add_Molar);
    if(data == TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Molar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 23:
    //2.2.2 PreMolar
    address = TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Add_PreMolar;
    data = TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.PreMolar;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Add_PreMolar);
    if(data == TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.PreMolar)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 24:
    //2.2.3 Incisor
    address = TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Add_Incisor;
    data = TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Incisor;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    data = WORD_VAL(TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Add_Incisor);
    if(data == TimerConfig.JawLowerConfig.RVGConfig.ChildConfig.Incisor)
    {
      FlashWriteIndex++;
    } else
    {
      FaultCounter++;
    }
    break;
    
      case 25:
    //Store the value of Mode selection
    address = JawSelectionAddress;
    byte_data = JawSelection;
    FlashProg(address, byte_data);
    
    
    address = RVGFilmAddress;
    byte_data = RVGFilmState;
    FlashProg(address, byte_data);    
    
    address = AdultChildAddress;
    byte_data = AdultChildState;
    FlashProg(address, byte_data);    
    
    address = SelectionAddress;
    byte_data = Selection;
    FlashProg(address, byte_data);                
    
    //Store the value of Pause time
    address = TimerConfig.Add_PauseTime;
    data = TimerConfig.PauseTime;
    byte_data = data & 0xff;
    FlashProg(address, byte_data);
    byte_data2 = (byte)((data & 0xff00)>>8);
    address++;
    FlashProg(address, byte_data2);
    
    //Check 
    data = *RVGFilmAddress;
    if(data == RVGFilmState)
    {
      data = *AdultChildAddress;
      if(data == AdultChildState)
      {
        data = *SelectionAddress;
        if(data == Selection)
        {
          data = WORD_VAL(TimerConfig.Add_PauseTime);
          if(data == TimerConfig.PauseTime)
          { 
            FlashWriteIndex = 1;
            PrevJawSelection = JawSelection;
            PrevRVGFilmState = RVGFilmState;
            PrevAdultChildState = AdultChildState;
            PrevSelection = Selection;
            prev_PauseTime = TimerConfig.PauseTime;
            Behaviour_state = IDLE;
            BuzzState = BUZZ_OFF;
            SetBuzzer();
          } else
          {
            FaultCounter++;
          }
        } else
        {
          FaultCounter++;
        }
      } else
      {
        FaultCounter++;
      }
    } else
    {
      FaultCounter++;
    }              
   
    break;
    default:
    Behaviour_state = IDLE;
    FlashWriteIndex = 1;
    BuzzState = BUZZ_OFF;
    SetBuzzer();
    break;
    }
    if(FaultCounter >= 3)
    {
      Behaviour_state = IDLE;
      PrevRVGFilmState = RVGFilmState;
      PrevAdultChildState = AdultChildState;
      PrevSelection = Selection;
      prev_PauseTime = TimerConfig.PauseTime;
      FlashWriteIndex = 1;
      BuzzState = BUZZ_OFF;
      SetBuzzer();
      FaultCounter = 0;
    }   
    
  }
  UpdateADC_Values();
   
     //__RESET_WATCHDOG(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}

        

