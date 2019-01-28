typedef enum{
  TIMER_IDLE,
  TIMER_STARTED,
  TIMER_FINISHED 
}TimerState_en;

typedef struct{
  dword CounterVal;
  dword OVCount;
}tst_timestamp;

typedef struct{
  dword TimeReq;
  TimerState_en status;
  tst_timestamp StartTime;
  tst_timestamp StopTime;
  dword CurrentVal;
}tst_SWTimer;

#define MAX_SW_TIMERS  15
#define MAX_DWORD 0x10000
#define MAX_WORD  0x10000

extern void Init_Timer();
extern void DisableTimer();
extern void EnableTimer(word count);
extern byte StartTimer(word time);
//extern void InitThreadTimer();
extern void StartXPOSTimer(dword time);
extern void StopXPOSTimer();
extern void StartThreadTimer(byte idx, dword time);
extern void GetSWTimer();//byte id, byte dum);