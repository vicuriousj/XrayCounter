#include <MC9S08AW16.h>
#include "IO_HWL.h"
#include "ADC_handler.h"
#include "Button_handler.h"
#include "Behaviour.h"


extern byte PEN_TOUCHED;
ADC_Selection_en ADC_Selected = ADC_VOLATAGE;
/*
byte RetryCount = 0;
byte TotalRetryCount = 0;
byte TouchPollingStatus = 0;

POS_State_en TouchPosState = NO_QUERY;
tst_Position Position[TOUCH_SAMPLE_COUNT] = {0,0};
TouchSample_en Touch_sample = TOUCH_IDLE;
const PosWindow_st KeyBoardLimits[KEY_MAX] = { 
  
  {19,18,35,35,   KEY_INCISOR,  EVENT_INCISOR,    LED_D1, 0},//KEY_INCISOR
  
  {42,17,62,47,   KEY_PREMOLAR, EVENT_PREMOLAR,   LED_D2, 1},//KEY_PREMOLAR
  
  {66,18,89,43,  KEY_MOLAR,    EVENT_MOLAR,      LED_D3, 2},//KEY_MOLAR
  
  {20,55,30,74,   KEY_ADLTCHLD, EVENT_ADULTCHILD, LED_D6, 3},//KEY_ADLTCHLD
  
  {38,59,52,74,   KEY_INCREMENT,EVENT_INCREMENT,  LED_D5, 4},//KEY_INCREMENT
  
  {59,54,79,75,  KEY_DECREMENT,EVENT_DECREMENT,  LED_D4, 5},//KEY_DECREMENT  
  
  {15,93,28,110,  KEY_RVGFILM,  EVENT_RVGFILM,    LED_D7, 6},//KEY_RVGFILM
  
  {32,93,46,111,  KEY_PROGRAM,  EVENT_PROGRAM,    LED_D8, 7},//KEY_PROGRAM
  
  {46,93,64,114, KEY_SET,      EVENT_SET,        LED_D9, 8},//KEY_SET
  
};
*/
extern byte id;
extern dword Timer_id_CurrVal;
word VADC_VALUE = ADC_INVALID_VALUE;
word CONNECT_ADC_VALUE = ADC_INVALID_VALUE;
word samples[3];

const byte LongPressTimerId = 9;
const byte XPOS_PauseTimerId = 10;

const byte SetBlinkTimer = 11;

const byte ADC_ChannelSelectTimer = 12;
const byte InitRoutTimerId = 13;


/*
//Function handles data format read from the ADS7843
void PollTouchADC_X() {
  byte temp,indx;
  //S,A2,A1,A0,Mode,SER/DFR,PD1,PD0
  //1,0,0,1,0,1,0,0 x+(y position)
  //1,1,0,1,0,1,0,0 y+(x position)
  byte config_byteX = 0x98;//0x93;//0x94;
  //byte config_byteY = 0xd4;
  if(PollTouchADC(config_byteX,3) == 1) {
    TouchPosState = XPOS_QUERY;
  }
  
}

void PollTouchADC_Y() {
  //S,A2,A1,A0,Mode,SER/DFR,PD1,PD0
  //1,0,0,1,0,1,1,1 x+(y position)
  //1,1,0,1,0,1,1,1 y+(x position)
  //byte config_byteX = 0x94;
  byte config_byteY = 0xd8;//0xd3;//0xd4;
  if(PollTouchADC(config_byteY,3) == 1) {
    TouchPosState = YPOS_QUERY;    
  }
  
}

//Function clears the read states and reads the ADC till it receives 3 values
byte PollTouchADC(byte config_byte, const byte num_values) {
  byte temp,indx,ret_val = 0;
  //Keep the buffer ready by resetting the previous received values
  while(SPIReadDataSize() != 0) {
    __RESET_WATCHDOG();
    ReadSPIDataByte(&temp);
  }
  
  //Continue asking the ADC value till "num_values" bytes are received
  for(indx = 0;indx < 5;indx++) {
    
    if(TransmitSPIData(&config_byte, 1)) {

    }
    if(SPIReadDataSize() >= num_values) {
      ret_val = 1;
      break;
    }
  } 
  
  DisableSPI();
  return ret_val; 
}


//Reset the state value of Touch scanning
void ResetTouchState() {
  TouchPosState = NO_QUERY;
}

//Configure ADS7843 in power down mode.
byte SendTouchMode() {
  const byte config_touch = 0x88;//0x84;
  byte ret_val = 0;
  if((PEN_TOUCHED == 0)&&(TouchPosState == NO_QUERY)) {
    ret_val = PollTouchADC(config_touch,1);    
  }
  return ret_val;
}

//Function acts as a job function handling states of touch screen position
void PollTouchPosition() {
  word var1;
  
  do{
    
  TouchPollingStatus = 1;
  switch(TouchPosState) {
    case NO_QUERY:
    __RESET_WATCHDOG();
    PollTouchADC_X();    
    RetryCount = 0;
    break;
    
    case XPOS_QUERY:
    var1 = ReadADCValue(ADC_RESOLUTION);
    if(var1 > 0) {
      if(Touch_sample != TOUCH_IDLE) { 
        //Check if the obtained value is within limits        
        if((var1 > MAX_X_COORDINATE)||
        (var1 < MIN_X_COORDINATE)) {            
          RetryCount++;
          TotalRetryCount++;
        } else {
          Position[Touch_sample].X_value = var1;        
          Touch_sample++;
        }       
      }      
      //Check if all samples obtained
      if(Touch_sample < TOUCH_DEBOUNCED) {
        
        TouchPosState =  NO_QUERY;
        
      } else {
        //If either all samples are obtained or 
        //if very first sample is ignored        
        if(Touch_sample == TOUCH_IDLE) {
          TouchPosState =  NO_QUERY;
        }else {
          TouchPosState = XPOS_RESPONSED;
        }
        Touch_sample = 0;
      }
      
    }else {
      RetryCount++;
      TotalRetryCount++;
      if(RetryCount >= MAX_RETRY_COUNT) {
        TouchPosState = NO_QUERY;
      }
    }
    break;
    
    case XPOS_RESPONSED:
    __RESET_WATCHDOG();
    PollTouchADC_Y();
    RetryCount = 0;    
    break;
    
    case YPOS_QUERY:
    var1 = ReadADCValue(ADC_RESOLUTION);
    if(var1 > 0) {      
      //Check if obtained value is within limits      
      if((var1 > MAX_Y_COORDINATE)||
      (var1 < MIN_Y_COORDINATE)) {            
        RetryCount++;
        TotalRetryCount++;
      } else {
        Position[Touch_sample].Y_value = var1;
        Touch_sample++;
      }      
          
      //Check if all samples are obtained
      if(Touch_sample < (TOUCH_DEBOUNCED)) {
        
        TouchPosState =  XPOS_RESPONSED;
        
      } else {          
        TouchPosState = YPOS_RESPONSED;
      }
      
    }else {
      RetryCount++;
      TotalRetryCount++;
      if(RetryCount >= MAX_RETRY_COUNT) {
        TouchPosState = XPOS_RESPONSED;
      }
    }    
    break;
    
    case YPOS_RESPONSED:
    TouchPosState = NO_QUERY;
    PEN_TOUCHED = 0; 
        
    //Debounce
    __RESET_WATCHDOG();
    Debounce_position();
    __RESET_WATCHDOG();
    SendTouchMode();
    TotalRetryCount = 0;
    break;
  } 
  
  //Worst case exit after trying 10 times
  if(TotalRetryCount >= MASTER_RETRY_COUNT) {
    TouchPosState = NO_QUERY;
    PEN_TOUCHED = 0;
    Touch_sample = TOUCH_IDLE;
    __RESET_WATCHDOG();
    SendTouchMode();
    TotalRetryCount = 0;
  }
  __RESET_WATCHDOG(); 
  }while((Touch_sample != TOUCH_IDLE)&&(Touch_sample != TOUCH_RESOLVED));//TOUCH_DEBOUNCED)); 
  TouchPollingStatus = 0;
}

//Function to debounce the noisy touch coordinate values
void Debounce_position() {
  dword temp_sum_x = 0;
  dword temp_sum_y = 0;
  dword mean = 0;
  dword var_x = 0;
  dword var_y = 0;
  byte temp_index = 0;
  byte common_found = 1;
  
  //Reset the values
  Position[TOUCH_DEBOUNCED].X_value  = 0;
  Position[TOUCH_DEBOUNCED].Y_value  = 0;
  
  if(1){
    //!(PTED & 0x10)) {
      
    //Average the values read from the ADC
    //exclude first sample. It reads junk data due to the SendTouchMode function call
    for(temp_index = 1;temp_index < TOUCH_DEBOUNCED;temp_index++) {    
      temp_sum_x += Position[temp_index].X_value;
      temp_sum_y += Position[temp_index].Y_value;     
      
    }
    
    if((common_found == 1)){
      
      //Check the variance
      mean = temp_sum_x / (TOUCH_DEBOUNCED - 1);
      for(temp_index = 1;temp_index < TOUCH_DEBOUNCED;temp_index++) {
        if(mean >= Position[temp_index].X_value) {
          
          var_x += (((dword)(mean - Position[temp_index].X_value))*
                   ((dword)(mean - Position[temp_index].X_value)));
        } else {
          var_x += (((dword)(Position[temp_index].X_value - mean))*
                   ((dword)(Position[temp_index].X_value - mean)));
        }
      }
      var_x = var_x / (TOUCH_DEBOUNCED - 1);
      
      //Check if variance is acceptable
      if(var_x <= X_VARIANCE) {
            
        mean = temp_sum_y / (TOUCH_DEBOUNCED - 1);
        for(temp_index = 1;temp_index < TOUCH_DEBOUNCED;temp_index++) {      
          if(mean >= Position[temp_index].Y_value) {
            var_y += (((dword)(mean - Position[temp_index].Y_value))*
                     ((dword)(mean - Position[temp_index].Y_value)));
          } else {
            var_y += (((dword)(Position[temp_index].Y_value - mean))*
                     ((dword)(Position[temp_index].Y_value - mean)));
          }
        }
        var_y = var_y / (TOUCH_DEBOUNCED - 1);
        if(var_y <= Y_VARIANCE) {
          Position[TOUCH_DEBOUNCED].X_value = temp_sum_x / (TOUCH_DEBOUNCED - 1);
          Position[TOUCH_DEBOUNCED].Y_value = temp_sum_y / (TOUCH_DEBOUNCED - 1);
          Touch_sample = TOUCH_RESOLVED;
        } else {
          common_found = 0;
        }
      } else {
        common_found = 0;
      }
      
      
    }
    if((common_found == 0)){
    
      Position[TOUCH_DEBOUNCED].X_value = 0;
      Position[TOUCH_DEBOUNCED].Y_value = 0;
      Touch_sample = TOUCH_IDLE;
    }   
    
  }
}

//Interface for obtaining the touch event
tst_TouchEvent_st GetTouchEvent() {
  
  tst_TouchEvent_st ret_val = {EVENT_NO_ACTIVITY,{0,0}};
  if(Touch_sample == TOUCH_RESOLVED){
    
    //Evaluate event type
    //If the timer for the given event is already started,
    //ignore the event.      
    ret_val.Event = EVENT_TOUCH;
    ret_val.Touch_Position = Position[TOUCH_DEBOUNCED];    
    EvaluateKeyPosition(&ret_val);
    
    Touch_sample = TOUCH_IDLE; 
  }
  
  return ret_val;
}

//Function for look up of the position
Keys_en EvaluateKeyPosition(tst_TouchEvent_st * key) {
  byte indx;  
  if(key->Event == EVENT_TOUCH) {
    for(indx = 0;indx < KEY_MAX;indx++) {
      __RESET_WATCHDOG();
      if((KeyBoardLimits[indx].X_Min <= key->Touch_Position.X_value)&&
      ((KeyBoardLimits[indx].X_Max >= key->Touch_Position.X_value))) {
        if((KeyBoardLimits[indx].Y_Min <= key->Touch_Position.Y_value)&&
          ((KeyBoardLimits[indx].Y_Max >= key->Touch_Position.Y_value))) {
          key->Event = KeyBoardLimits[indx].TEvent;
          return KeyBoardLimits[indx].Key;
        }
      }
    }
  }
  return KEY_MAX;
}
  */


//Function to read data from SPI buffer in the format of ADC conversion
//argument type- 0 - 12bit mode
//               1 - 8bit mode
/*
For the 8-bit mode, the data format, 
result = ( ((msb & 0x7F) <<256) | lsb) >> 7 
For the 12-bit mode, the data format is: 
result = ( ((msb & 0x7F) <<256) | lsb) >> 3
Ref:http://e2e.ti.com/support/other_analog/touch/f/750/t/210483.aspx 

word ReadADCValue(byte format) {
  byte val1, val2, val3;
  word ret_val = 0;  
  
  if(ReadSPIDataByte(&val1) > 0) {
    if(ReadSPIDataByte(&val2) > 0) {
      if(ReadSPIDataByte(&val3) > 0) {
        ret_val = val3;
      }
    }
  }  
  return ret_val;
}
 */
//Function initializes the ADC channel no.1 for monitoring Vadc more than 2.5volts
void InitADC() {


  //Configure ADC1SC1
  //Configure for polling,continuous mode, channel AD1,
  //ConvComp(7),Interrupt_en(6), Continu_conv(5), Channel select(4-0)
  ADC1SC1 = 0x29;
  Delay(2);
  //Configure trigger mode
  //conv_active, conv_trig, compare_func, compare functionality >=
  ADC1SC2 = 0;
  
  //Configure the mode of operation and clock
  //ADC1CFG
  //ADLPC-lowpower(7),ADIV-clock divide(6-5), ADLSMP-Longsample(4), MODE-Convmode(3-2),ADICLK-ipclock(1-0)
  ADC1CFG = 0x98;
  //Configure the pin to ADC mode
  //For channel no.1 2nd bit
  APCTL1 = 0x02; 
  
  //ADC1RH and ADC1RL have converted values
  ADC_Selected = ADC_VOLATAGE;
  
  id = ADC_ChannelSelectTimer;
  Timer_id_CurrVal = 250;
  StartThreadTimer(0,0);
  
}


//Function initializes the ADC channel no.1 for monitoring Vadc more than 2.5volts
void InitErrorADC() {

  //Configure ADC1SC1
  //Configure for polling,continuous mode, channel AD1,
  //ConvComp(7),Interrupt_en(6), Continu_conv(5), Channel select(4-0)
  ADC1SC1 = 0x21;//0x21;
  Delay(2);
  //Configure trigger mode
  //conv_active, conv_trig, compare_func, compare functionality >=
  ADC1SC2 = 0;
  
  //Configure the mode of operation and clock
  //ADC1CFG
  //ADLPC-lowpower(7),ADIV-clock divide(6-5), ADLSMP-Longsample(4), MODE-Convmode(3-2),ADICLK-ipclock(1-0)
  ADC1CFG = 0x98;
  //Configure the pin to ADC mode
  //For channel no.1 2nd bit
  APCTL1 = 0x02;
  
  //ADC1RH and ADC1RL have converted values
  ADC_Selected = ADC_VOLATAGE;
  
  id = ADC_ChannelSelectTimer;
  Timer_id_CurrVal = 1000;
  StartThreadTimer(0,0);
  
}

//Check if ADC value exceeds the limit
VoltageStatus_en IsVoltageHigh() {
  
  word reading = 0;
  VoltageStatus_en ret_val = ADC_IDLE;  
  
  
  reading = VADC_VALUE;
  if(reading < ADC_INVALID_VALUE)
  {
    
    if(reading >= VADC_THRESHOLD) {
      ret_val = VOLTAGE_EXCEED_LIMIT;
    } else if((reading >= VADC_LOWER_THRESHOLD)&&(reading < VADC_THRESHOLD)) {
      ret_val = VOLTAGE_WITHIN_LIMIT;
    } else
    {
      ret_val = VOLTAGE_BELOW_LIMIT;
    }
  }else
  {
    ret_val = ADC_CONVERTING;
  }  
    
  return ret_val;
}

//Function initializes the ADC channel no.2 for monitoring X-Ray head connection by
//checking if the voltage is more than 1volt.
void InitConnectionADC() {

    //Configure ADC1SC1
    //Configure for polling,continuous mode, channel AD1,
    //ConvComp(7),Interrupt_en(6), Continu_conv(5), Channel select(4-0)
    ADC1SC1 = 0x21;//AD9
    Delay(2);
    //Configure trigger mode
    //conv_active, conv_trig, compare_func, compare functionality >=
    ADC1SC2 = 0;
    
    //Configure the mode of operation and clock
    //ADC1CFG
    //ADLPC-lowpower(7),ADIV-clock divide(6-5), ADLSMP-Longsample(4), MODE-Convmode(3-2),ADICLK-ipclock(1-0)
    ADC1CFG = 0x98;
    //Configure the pin to ADC mode
    //For channel no.2 3rd bit
    APCTL1 = 0x04;
    
    //ADC1RH and ADC1RL have converted values
    ADC_Selected = ADC_CONNECTION;
    
    id = ADC_ChannelSelectTimer;
    Timer_id_CurrVal = 1000;
    StartThreadTimer(0,0);
   
}


//Function to check error state for Error1, XRAY head loose connection
VoltageStatus_en IsHeadDisconnected()
{
  word reading = 0;
  VoltageStatus_en ret_val = ADC_IDLE; 
  
  reading = CONNECT_ADC_VALUE;
    
  if(reading < VCONNECT_LOW_THRESHOLD) {
    ret_val = VOLTAGE_BELOW_LIMIT;
  } else if(reading < ADC_INVALID_VALUE)
  {
    ret_val = VOLTAGE_WITHIN_LIMIT;
  } else
  {
    ret_val = ADC_CONVERTING;
  }
    
  return ret_val;
}

//Generic function which updates both ADC values in their buffers.
void UpdateADC_Values()
{
  word reading;
  byte temp = 0;
  static byte samp_index = 0;
  static word prev_adc_val = 0;
  if(ADC_Selected == ADC_CONNECTION)
  {
    //If conversion is complete    
    if(ADC1SC1 & 0x80) {
      reading = ADC1RH;
      reading = reading << 8;
      reading |= ADC1RL;           
     ADC1SC1 = 0x09;//AD9
     
      if(((samp_index & 0x07) == 2)&&(prev_adc_val == reading))
      {   
        CONNECT_ADC_VALUE = reading;
        InitErrorADC();                    
        samp_index = 0;
      } else
      {           
          if(prev_adc_val == reading)
          {
            samp_index++;    
          } else
          {
            prev_adc_val = reading;
            samp_index &= 0xf8;
            temp = (samp_index & 0xf8)>>3;
            temp++;
            samp_index &= 0x07;
            samp_index |= (temp<<3);
          }  
      }
      if((samp_index & 0xf8) >= 0xf0)
      {               
        InitErrorADC();
        samp_index = 0;
      }
    }
  } 
  else
  {
     //If conversion is complete
    if(ADC1SC1 & 0x80) {
      reading = ADC1RH;
      reading = reading << 8;
      reading |= ADC1RL;
      ADC1SC1 = 0x01;//0x21;
      //samples[samp_index] = reading;
      if(((samp_index & 0x07) == 2)&&(prev_adc_val == reading))
      {   
        VADC_VALUE = reading;
        InitConnectionADC();      
        samp_index = 0;
      } else
      {         
          if(prev_adc_val == reading)
          {
            samp_index++;    
          } else
          {
            prev_adc_val = reading;
            samp_index &= 0xf8;
            temp = (samp_index & 0xf8)>>3;
            temp++;
            samp_index &= 0x07;
            samp_index |= (temp<<3);
          }         
      }
      if((samp_index & 0xf8) >= 0xf0)
      {               
        InitConnectionADC();
        samp_index = 0;
      }
    }
  }
}
 