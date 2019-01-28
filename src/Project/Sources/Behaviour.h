
//Macro to merge two byte values from a given address and form a Little endian WORD value
#define WORD_VAL(address) ((*address)|((*(address+1))<<8))
typedef enum{
  IDLE,
  MOLAR,
  PREMOLAR,
  INCISOR,
  PROGRAM_PRESS,
  PROGRAM_WAIT_LONGPRESS,
  PROGRAM_RELEASE,
  SET_WAIT_LONGPRESS,
  SET_PRESS,
  SET,
  STOP_SET,
  PAUSE_TIME,  
  PAUSE_TIME_EDIT,
  PAUSE_TIME_CONFIRM,
}ControlState_en;

typedef enum{
  NO_SELECT,
  MOLAR_SELECT,
  PREMOLAR_SELECT,
  INCISOR_SELECT,
}SelectedVal_en;

typedef enum
{
  JAW_UPPER,
  JAW_LOWER
}JawSelection_en;

typedef enum{
  FILM,
  RVG
}RVG_Film_en;

typedef enum{
  ADULT,
  CHILD
}Adult_Child_en;

typedef enum{
  XPOS_IDLE,
  XPOS_TCON_WAIT,
  XPOS_ZDC_WAIT,  
  XPOS_ON,  
  XPOS_IDLE_BEFORE_PAUSE,
  XPOS_PAUSE_TIME,
}XPOS_State_en;

typedef enum
{
  NO_ERRORS,
  ERROR_1,
  ERROR_2,//Trigger released when EXPOSURE ON state
  ERROR_3,
  ERROR_4,//Low voltage
  ERROR_5,//High voltage
  
}XErrorStates;

typedef struct{
  word Molar;  
  word PreMolar;  
  word Incisor;
  byte * Add_Molar;
  byte * Add_PreMolar;
  byte * Add_Incisor;
}MemConfig_st;


typedef struct{
  MemConfig_st AdultConfig;
  MemConfig_st ChildConfig;
}FilmRVGConfig_st;


typedef struct{
  FilmRVGConfig_st FilmConfig;
  FilmRVGConfig_st RVGConfig;
  
}TimerConfig_st;

typedef struct
{
  TimerConfig_st JawUpperConfig;
  TimerConfig_st JawLowerConfig;
  word PauseTime;
  byte * Add_PauseTime;
  
}JawConfig_st;

typedef enum{
  STATE_IDLE,
  STATE_ENABLED,
  
}KeyEnable_en;

typedef enum{
  HMI_IDLE,
  HMI_INCREMENT,
  HMI_DECREMENT,
}HMI_State_en;

typedef enum
{
  ADC_VOLATAGE,
  ADC_CONNECTION,
}ADC_Selection_en;

#define MAX_POSSIBLE_DISPLAY_VAL 9999
#define MAX_PAUSE_VALUE 900
#define SEG_DISP_1  PTFD_PTFD0
#define SEG_DISP_2  PTFD_PTFD1
#define SEG_DISP_3  PTFD_PTFD4
#define SEG_DISP_4  PTFD_PTFD5

#define KEY_STATE_TIMEOUT (120)//70)
#define KEY_SET_LONGPRESS (1000)
#define XPOS_PAUSE_TIMEOUT (1000)//900)
#define CHAR1_DOT_ENABLE  0x01;
#define CHAR2_DOT_ENABLE  0x02;
#define CHAR3_DOT_ENABLE  0x04;
#define CHAR4_DOT_ENABLE  0x08;

#define DISPLAY_FREQ  4//Update display every 1ms
#define INCR_STEP1_LIMIT  10
#define INCR_STEP10_LIMIT  30
#define INCR_STEP100_LIMIT  40
#define INCR_STEP1000_LIMIT  50

  

extern void InitBehaviour();
extern void BehaviourJob();
extern void DisplayHMI();
extern void ChangeLEDState(LEDs_en led_indx, byte val);
extern void ChangeSETLed(byte val);
extern void UpdateDispCount();
extern void ResetKeyStates();
extern void UpdateDisplayString();
extern void RestoreDispCount();
extern void ChangeSelectionStates();