#define INIT_ROUTINE_TIME_STEP  500
#define INIT_ROUTINE_ROLL_STEP  150

#define MAX_EACH_CHAR_SEG_ROLL_INDEX  8
#define MAX_ALL_SEG_ROLL_INDEX  (4 * MAX_EACH_CHAR_SEG_ROLL_INDEX)
#define MAX_LED_ROLL_INDEX  LED_ENUM_MAX

typedef enum
{
  ALL_ON,
  SEVEN_SEG_ON,
  LEDS_ON,
  SEVEN_SEG_ROLL,
  LED_ROLL,
  TOTAL_INIT_ROUTINES,
  INVALID
}InitRoutType_en;

extern void InitRoutine();
