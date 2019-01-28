#define SWITCHES_COUNT 20

typedef enum{

  PORT_A = 0,
  PORT_B = 1,
  PORT_C = 2,
  PORT_D = 3,
  PORT_E = 4,
  PORT_F = 5,
  PORT_G = 6,
  
  MAX_PORT
}PORTS;

#define INVALID_CHANNEL 255

#define SW1 11//PTED_PTED4
#define SW2 10//PTED_PTED3
#define SW3 9//PTED_PTED2

#define SW4 15//PTFD_PTFD5

#define SW5 16//PTGD_PTGD0

#define SW6 14//PTED_PTED7
#define SW7 13//PTED_PTED6
#define SW8 12//PTED_PTED5

#define SW9 17//PTGD_PTGD1

#define SW10 18//PTGD_PTGD2

#define SW11 0//PTAD_PTAD0
#define SW12 1//PTAD_PTAD1

#define SW13 2//PTBD_PTBD0
#define SW14 3//PTBD_PTBD1
#define SW15 4//PTBD_PTBD2
#define SW16 5//PTBD_PTBD3

#define SW17 6//PTDD_PTDD0
#define SW18 7//PTDD_PTDD2
#define SW19 8//PTDD_PTDD3

#define SW20 19//PTGD_PTGD3

//duplicating SW in S
//#define

#define IO_DIR_INPUT  1
#define IO_DIR_OUTPUT  0

#define PULLUP_ENABLE  1
#define PULLUP_DISABLE  0

#define MAX_PWM1_VAL 35
#define MIN_PWM1_VAL 0

#define MAX_PWM2_VAL 10
#define MIN_PWM2_VAL 0

typedef struct{
  
  byte Pin_num;
  byte Direction:1;
  byte Pull_Up_En:1;
  byte Default_val:1;  
  
}IO_PIN_INFO;

typedef struct{

  PORTS Port_num;
  byte * HW_Port;
  byte * Port_Mode;
  byte * Port_Pullup;
  byte UsedPinCount;
  IO_PIN_INFO Pins[8];  
  
}PORT_CONF;

typedef enum {
  BUZZ_OFF,
  BUZZ_TIMED_ON,
  BUZZ_TIMED_OFF,
  BUZZ_CONTINUOUS,
}Buzz_en;

typedef struct TimeProg_st {
  dword Molar;
  dword PreMolar;
  dword Incisor;
} tst_TimeProg;

typedef struct AgeGroupPrg_st{
  tst_TimeProg Adult;
  tst_TimeProg Child;
}tst_AgeGroupPrg;

typedef struct KeyProg_st {
   tst_AgeGroupPrg RVG;
   tst_AgeGroupPrg Film;
}tst_KeyProg_st;

typedef enum{
  LED_D1 = 0x01,//D7 //A //Incisor
  LED_D2 = 0x02,//D9 //B //Premolar
  LED_D3 = 0x04,//D10//C //RVG
  LED_D4 = 0x08,//D12//D //Film
  LED_D5 = 0x10,//D14//E //Child
  LED_D6 = 0x20,//D15//F //Upper Jaw
  LED_D7 = 0x40,//D17//G //Adult
  LED_D8 = 0x80,//D18//H //Molar
  LED_D9 = 0x100,//PST
  LED_D10 = 0x200,//LDE1
  LED_D11 = 0x400,//EXP
  LED_D12 = 0x800,//ERR
  LED_D13 = 0x1000,//START
  
  LED_MAX = 14
} LED_States_en;

typedef enum{
  
  PEN_IDLE,
  PEN_ACTIVE,
  PEN_DEBOUNCING,
  PEN_DETECTED,
  
} PEN_State_en;


#define LED_SEGMENT_MUX PTED//PE0-PE7
#define LED_SEG_MUX_RESET  0x00

#define LED_STATES  PTCD//PC1-PC5
#define LED_STATES_RESET  0xc1

#define LED_SEG_SELECT  PTCD_PTCD0
#define SEVEN_SEG_SELECT  PTFD
#define SEVEN_SEG_RESET   0xcc

#define BUZZER_ON_TIME (500 * 2)
#define BUZZER_OFF_TIME (250 * 2)
#define PEN_DEBOUNCE_TIME 3//3ms


void InitPortPins();
byte SetChannel_Value(byte channel_v, byte value);
void DiagnosticTest();
void PollPENIRQ();
void UpdateLED(word config);
void SetBuzzer();