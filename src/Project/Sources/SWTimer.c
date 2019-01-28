#include <MC9S08AW16.h>
#include "IO_HWL.h"
#include "ADC_handler.h"
#include "Button_handler.h"
#include "Behaviour.h"
#include "Timer.h"
#include "SWTimer.h"

tst_SWTimer Timers[MAX_SW_TIMERS];
dword Timer_id_CurrVal;
extern byte id;
extern dword ThreadTimerOFCount;
extern word ThreadTimeCounter;


//Start timer
void StartThreadTimer(byte id1, dword time1) {
  dword time = Timer_id_CurrVal;
  if(id < MAX_SW_TIMERS) {    
    Timers[id].TimeReq = time;
    Timers[id].StartTime.CounterVal = ThreadTimeCounter;
    Timers[id].StartTime.OVCount = ThreadTimerOFCount;
    
    //Required values
    Timers[id].StopTime.CounterVal = ((time + ThreadTimeCounter)% 0x10000);
    Timers[id].StopTime.OVCount = ThreadTimerOFCount + ((time + ThreadTimeCounter)/ 0x10000);    
    Timers[id].status = TIMER_STARTED;
    Timers[id].CurrentVal = 0;
  }
}
//Timer interface for thread timer
//returns the remaining time
void GetSWTimer(){
  //byte id, byte dum) {  
  dword count_required = 0;
  dword count_elapsed = 0;
  Timer_id_CurrVal = 0;
  if((id < MAX_SW_TIMERS)&&(Timers[id].status == TIMER_STARTED)) {
    //Check if the overflow count is reached
    if(((Timers[id].StopTime.OVCount > Timers[id].StartTime.OVCount)&&(ThreadTimerOFCount > Timers[id].StopTime.OVCount))||
    ((Timers[id].StopTime.OVCount < Timers[id].StartTime.OVCount)&&((ThreadTimerOFCount < Timers[id].StartTime.OVCount)&&(ThreadTimerOFCount > Timers[id].StopTime.OVCount)))) {
      
      Timers[id].status = TIMER_IDLE;
      Timers[id].CurrentVal = 0;
    }else if(ThreadTimerOFCount == Timers[id].StopTime.OVCount) {
      if(Timers[id].StopTime.CounterVal <= ThreadTimeCounter) {        
        Timers[id].CurrentVal = 0;
        Timers[id].status = TIMER_IDLE;
      } else{
        Timers[id].CurrentVal = Timers[id].StopTime.CounterVal - ThreadTimeCounter;
      }
    }else{
      if(Timers[id].StopTime.OVCount < Timers[id].StartTime.OVCount) {
        count_required = ((MAX_DWORD - Timers[id].StartTime.OVCount)+ Timers[id].StopTime.OVCount)*MAX_WORD+
                          (Timers[id].StopTime.CounterVal - Timers[id].StartTime.CounterVal);
        if(ThreadTimerOFCount < Timers[id].StartTime.OVCount) {            
          count_elapsed = ((MAX_DWORD - Timers[id].StartTime.OVCount)+ ThreadTimerOFCount)*MAX_WORD+
                            (ThreadTimeCounter - Timers[id].StartTime.CounterVal);
        }else {
          count_elapsed = (ThreadTimerOFCount - Timers[id].StartTime.OVCount)*MAX_WORD + 
                          (ThreadTimeCounter - Timers[id].StartTime.CounterVal);
        }                                
      } else {
        count_elapsed = (ThreadTimerOFCount - Timers[id].StartTime.OVCount)*MAX_WORD + 
                          (ThreadTimeCounter - Timers[id].StartTime.CounterVal);
      }
      Timers[id].CurrentVal = Timers[id].TimeReq - count_elapsed;
    }
      
  }
  Timer_id_CurrVal = Timers[id].CurrentVal;
  
  //return Timers[id].CurrentVal;
  
} 