typedef enum{

  NO_QUERY,
  XPOS_QUERY,
  YPOS_QUERY,
  XPOS_RESPONSED,
  YPOS_RESPONSED  
}POS_State_en;

typedef enum{
  
  TOUCH_1,
  TOUCH_2,
  TOUCH_3,
  TOUCH_4,
  TOUCH_5,
  //TOUCH_6,
  TOUCH_DEBOUNCED = 5,
  TOUCH_SAMPLE_COUNT = 6,
  TOUCH_RESOLVED,
  TOUCH_IDLE = 255,
  
}TouchSample_en;

/*
typedef enum{
  KEY_INCISOR,
  KEY_PREMOLAR,
  KEY_MOLAR,  
  KEY_ADLTCHLD,
  KEY_INCREMENT,
  KEY_DECREMENT,  
  KEY_RVGFILM,
  KEY_PROGRAM,  
  KEY_SET,
  
  KEY_MAX
}Keys_en;

//Touch events after debouncing
typedef enum enum_TouchEvent{
  EVENT_NO_ACTIVITY,
  EVENT_TOUCH,
  EVENT_LONG_TOUCH,
  EVENT_DRAG,
  EVENT_MOLAR,
  EVENT_PREMOLAR,
  EVENT_INCISOR,
  EVENT_ADULTCHILD,
  EVENT_PROGRAM,
  EVENT_SET,
  EVENT_RVGFILM,
  EVENT_INCREMENT,
  EVENT_DECREMENT,  
  
}TouchEvent_en;

typedef struct{
  word X_value;
  word Y_value;
  
}tst_Position;

typedef struct{
  TouchEvent_en Event;
  tst_Position Touch_Position;
}tst_TouchEvent_st;

typedef struct{
  
  word X_Min;
  word Y_Min;
  word X_Max;
  word Y_Max;
  Keys_en Key;
  TouchEvent_en TEvent;
  LED_States_en LED;
  byte TimerID;
  
}PosWindow_st;
*/

typedef enum{
  ADC_CONVERTING,
  VOLTAGE_BELOW_LIMIT,
  VOLTAGE_WITHIN_LIMIT,
  VOLTAGE_EXCEED_LIMIT,
  ADC_IDLE,
}VoltageStatus_en;

#define MAX_RETRY_COUNT 5
#define MASTER_RETRY_COUNT  10

#define MAX_X_COORDINATE  120
#define MAX_Y_COORDINATE  120//230
#define MIN_X_COORDINATE  10
#define MIN_Y_COORDINATE  10
#define X_VARIANCE  1//4//100//square of 10
#define Y_VARIANCE  1//4//100//square of 10

#define ADC_RESOLUTION  1//0-12 bit, 1-8 bit

//Ideal Vadc voltage 2.545V(ADC = 525).. Error condition should be for values greater than 2.65V(546)
//current reading 3.10v
#define VADC_THRESHOLD  650//546
#define VADC_LOWER_THRESHOLD  306//1.5V//250
#define VCONNECT_LOW_THRESHOLD  204//10bit ADC, 1023 = 5V, 1V = 204
#define ADC_INVALID_VALUE 65535

/*extern byte PollTouchADC(byte config_byte, const byte num_values);
extern void PollTouchADC_X();
extern void PollTouchADC_Y();
extern void PollTouchPosition();
extern void ResetTouchState();
extern byte SendTouchMode();
extern void Debounce_position();
extern Keys_en EvaluateKeyPosition(tst_TouchEvent_st * key);
extern tst_TouchEvent_st GetTouchEvent();
*/
extern word ReadADCValue(byte format);
extern void InitErrorADC();
extern VoltageStatus_en IsVoltageHigh();
extern void InitConnectionADC();
extern VoltageStatus_en IsHeadDisconnected();
extern void UpdateADC_Values();
extern void InitADC();