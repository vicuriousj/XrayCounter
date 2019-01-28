#include <MC9S08AW16.h>
#include <hidef.h> /* for EnableInterrupts macro */

#define CRITICAL()         __asm tpa; __asm psha; __asm sei
#define CRITICAL_RESTORE() __asm pula; __asm tap

//Erase flash page where the given address exists.
byte EraseFlash(byte* address)
{
  byte ret_val = 1;
  //DisableInterrupts;
  CRITICAL();
  
  //Erase the page of the address start
  //Latch the address and data to be erased
  *address = 0;  
  
  //To erase, initially clear the PVIOL and ACCERR flags in FSTAT
  FSTAT = 0x30;
  
  //Erase command
  FCMD = 0x40;
  
  //Start command execution
  //Set the bit FCBEF
  FSTAT |= 0x80;
  
  _asm nop;
  _asm nop;
  _asm nop;
  _asm nop;
  
  //wait for 4ms till the FCCF bit is set
  while(!(FSTAT & 0x40))
  {
    //do nothing
  }
  
  if(FSTAT & 0x30)
  {
    ret_val = 0;
  }
  //EnableInterrupts;
  CRITICAL_RESTORE();
  
  return ret_val;
}

//Function erases the page for writing and then writes the data needed.
byte WriteFlash(byte* address, byte data)
{
  //Start address
  //word address = (word)TimerConfig.FilmConfig.AdultConfig.Add_Molar;
  //byte* temp_addressptr = (byte*)address;
  //byte bytecount = 24;
  //word data = TimerConfig.FilmConfig.AdultConfig.Molar;
  byte ret_val = 1;
  
  //DisableInterrupts;
  CRITICAL();
  
  //Erase the page of the address start
  //Latch the address and data to be erased
  *address = data;  
  
  //Start write sequence
  FSTAT = 0x30;
  
  //Byte program command
  FCMD = 0x20;
  
  //Start command execution
  //Set the bit FCBEF
  FSTAT |= 0x80;
  
  _asm nop;
  _asm nop;
  _asm nop;
  _asm nop;
  
  //wait for 4ms till the FCCF bit is set
  while(!(FSTAT & 0x40))
  {
    //do nothing
  }
  
  if(FSTAT & 0x30)
  {
    ret_val = 0;
  }
  //Set the bit FCBEF
  //FSTAT |= 0x80;
  
  //EnableInterrupts;
  CRITICAL_RESTORE();
  
  return ret_val;
  
}
