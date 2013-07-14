//
// gogo40.c - Contains the core functions of the gogo board firmware
//
// Copyright (C) 2001-2008 Massachusetts Institute of Technology
// Contact   Arnan (Roger) Sipiatkiat [arnans@gmail.com]
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation version 2.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#case

#include <18F66J50.H>
#device ADC=10 *=16

#FUSES NOWDT, WDT128, PLL5, NOXINST, HSPLL, NOIESO, NOPROTECT, NOCPUDIV
#use delay(clock=48M)


#byte OSCTUNE = 0xF9B
#bit PLLEN=OSCTUNE.6


#byte INTCON2 = 0xFF1
#bit RBPU=INTCON2.7   // Port B internal pull-up enable bit (0=enable)


                         



#include <usb_cdc.h>
#include <rpt10.H>
#include <bootload.h>

//#use rs232(baud=9600,UART1)
#use i2c(master, I2C1, FORCE_HW)
#use rs232(baud=9600, UART1)

#include <logovm.h>
#include <stdlib.H>
#include <i2cDisplay.h>
#include <i2c.h>   // i2c read/write routines
#include <ds1307.h>   // real time clock module
#include <7segment.h> // on-baord 7-segment display driver



#use fast_io(A)
#use fast_io(B)
#use fast_io(C)  
#use fast_io(D)
#use fast_io(E)
#use fast_io(F)
#use fast_io(G)


#define HARDWARE_ID1     0x01   // 02 = Pi-Topping Board (for the Raspberry Pi)
#define HARDWARE_ID2     0x10   // 10 = version 1.0
#define FIRMWARE_ID      1   // Firmware version



#define defaultPort      0

#define channelSwitchDelay   100   // delay time in us after switching adc channels
                              // Don't decrease this value without testing.
                              // If the delay is too short (i.e. 10us) the adc won't
                              // have enough time to stabilize before reading the
                              // next channel.

/// Timer 1 Counter
/// This value should be 3036 ( 65536 - 0.1/(4/20000000) * 8)
/// where 0.1 is the time in seconds we want the ISR to be called
/// 4/20000000 is the time each counter takes to tick
/// 8 is the timer divider ... see setup_timer_1()

//#define T1_COUNTER      3036     // tick rate = 1.60/8 uSec (value according to the math)

#define T0_COUNTER      56161      // at 48Mhz, and prescale=128 the interrupt period is
                                   // calculated from 1/48 * 4 * 128 * (65536-56161)= 100 ms
#define PWM_PERIOD      27135      // 65535-38400. Used in Timer1, which gives a 25.6 ms period



#define  CMD_TIMEOUT_PERIOD  4     // determins how long befor the board will reset
                                    // the command state. Units in 1/10 of a second


// RESERVED_MEMORY_START must be the same as RECORD_BASE_ADDRESS in memoryMap.h
#define RESERVED_MEMORY_START    0x5800 // must be a multiple of getenv("FLASH_ERASE_SIZE"), which is 1024 in the pic18F66J50
#define RESERVED_MEMORY_END      0xFFF0

#if defined(__PCM__)
   // reserve memory for the bootloader. This allows firmware updates via the serial port
   #org 0x1d00, 0x1FFF void loader16F876(void) {}
   
   // This memory area is used to store Cricket Logo commands
   #org 0x1c00, 0x1cff void cricketLogoMemoryArea(void) {}

#elif defined(__PCH__)
   // reserve memory for the bootloader. This allows firmware updates via the serial port
///   #org 0x6500, 0x7FFF void loader16F876(void) {}  // these are byte addresses. 
                                                   // equiv word addresses are 3D00 - 3FFF
   
   // This memory area is used to store Cricket Logo commands
///   #org 0x7200, 0x79DF void cricketLogoMemoryArea(void) {} // equiv word addresses are 3900 - 3DEF
      #org RESERVED_MEMORY_START, RESERVED_MEMORY_END void reserved(void) {} 
                                                 // do not allow gogo code to use the upper parts of
                                                 // the flash memory. They are reserved for data logging
                                                 // array storage, and the logo code. See memoryMap.h
                                                 // for memory allocation details.


#endif



/////////////////////////////////////////////////////////////////
//  Function Declaration
/////////////////////////////////////////////////////////////////

void startStopLogoProcedures(void); 
void stopLogoProcedures(void);


void setHigh(IOPointer Pin);
void setLow(IOPointer Pin);
int  readPin(IOPointer Pin);

short getBit(int InByte, int BitNo);
void setBit(int *InByte, int BitNo);
void clearBit(int *InByte, int BitNo);

void Ping(int Param);
void TalkToMotor(int MotorBits);
void MotorControl(int MotorCmd);
void SetMotorPower(int Power);
void ChangeMotorPower(int delta);
void createPWMVectorTable(void);
//void sortMtrDuty();
void SetMotorMode(int motorMode); // normal or servo

void ENHigh(int groupNo);
void ENLow(int groupNo);

void MotorON(int MotorNo);
void MotorOFF(int MotorNo);
void MotorRD(int MotorNo);
void MotorThisWay(int MotorNo);
void MotorThatWay(int MotorNo);
void MotorCoast(int MotorNo);
void miscControl(int cur_param, int cur_ext, int cur_ext_byte);

void beep();

void SetBurstMode(int SensorBits, int Mode);
void DoSensorStuff();
// this prints the result back to the PC
//#inline int16 outputSensor(int Target, int readMode);
// this one just returns the sensor value
unsigned int16 readSensor(int sensorNo);
long getSensorVal();
void switchAdcChannel(int channelNo);

void ProcessInput();
void ProcessRFInput();
//int  process_input();
//void Add_to_CMD(int InByte);
//void EndCMD();
//byte get_cmd_buff(int *cmd_ptr);
void init_variables();
void intro ();
void Halt();
void initBoard();

void DoDisplayModuleStuff();
void DoMotorStuff();

//////////////////////////////////////////
// Flash memory routines
/////////////////////////////////////////

void FLASHSetWordAddress(int16 address);
void FLASHBufferedWrite(int16 InByte) ;
void FLASHFlushBuffer();
void FLASHWrite(int16);
void writeFLASH(int16 memoryBlockAddress, int16 positionInMemoryBlock, int16 len, int *Buffer);


timer2ISR();

/////////////////////////////////////////////////////////////////
//  Global Variables
/////////////////////////////////////////////////////////////////


//IOPointer  MotorENPins [MotorCount]={  MTR1_EN, MTR2_EN, MTR3_EN, MTR4_EN};
IOPointer  MotorCWPins [MotorCount]={  MTR1_CW, MTR2_CW, MTR3_CW, MTR4_CW};
IOPointer  MotorCCPins [MotorCount]={  MTR1_CC, MTR2_CC, MTR3_CC, MTR4_CC};

unsigned int CMD_STATE;

int gbl_cur_cmd, gbl_cur_param, gbl_cur_ext, gbl_cur_ext_byte;
int gblExtCMDBuffer[32];   // buffer to hold the gogo extended command stream
int1 gblExtCmdMode=0;   // flag. Tells us if the command recieved is an extended command.
int gblExtCmdLen;
int gblExtCmdBufferIndex;


int gblSensorPortMap[8] = {11,10,7,3,2,1,0,4}; // this array maps the sensor number to the 
                                            // physical sensor port used on the PIC

int gblBurstModeBits;
int gblBurstModeCounter=0;   // tracks which sensor is the current burst mode sensor

int1 gblSlowBurstMode=0;  // determinds which burst mode we're in (0=normal, 1=slow)
                          // as of GoGo 4.0, this option has no effect. 
                          
int1 gblBurstModeTimerHasTicked=0;  // ticks every 1/72 sec (by timer0)
int gblCurSensorChannel;




int gblMotorMode=0b00000000;   // default to normal mode
int gblActiveMotors;
int gblMotorDir=0;
int gblMotorONOFF = 0;
int gblMtrDuty[MotorCount+1] = {0xff,0xff,0xff,0xff,0xff};  // Motor PWM Duty cycle
int gblMtrPrevDuty[MotorCount+1] = {0xff,0xff,0xff,0xff,0xff};  //Prev PWM Duty cycle -> to detect change 
int1 gblMtrNeedToRecreateMtrDutyVector = 0;  // True when gblMtrDuty != gblMtrPrevDuty
int1 gblAtEndOfPWMCycle = 0; // True when at end of a PWM cycle. This is when we can
                             // update the duty vector 
//int gblMtrDutyIndex[MotorCount+1] = {0,1,2,3,4}; // index to the sorted gblMtrDuty[]
//unsigned int gblTimer0Counter = MotorCount; // Motor duty cycle counter.
//unsigned int gblDutyCycleFlag = 0; // used to find the next duty cycle in timer0
//unsigned int gblCurrentDutyIndex = 0; // keeps track of the current duty cycle being used.

int1 flgNeedToTurnOffAllMotors=0; // flag to tell the main loop to turn off all motors

//////////////////////////////////////////////////////////
// PWM variables
//////////////////////////////////////////////////////////
int16 gblPWMVector[4] = {0,0,0,0};
int   gblPWMVectorPorts[4] = {0,0,0,0};
int   gblPWMVectorLen = 0;
int16 gblPWMTerminatingVector = PWM_PERIOD;
int   gblPWMVectorIndexCounter = 0;




// These two variables are for the NEWSERIAL, SERIAL commands in the Logo VM
unsigned char gblMostRecentlyReceivedByte;
int1 gblNewByteHasArrivedFlag = 0;



int1 gblLogoIsRunning = 0;     // flags if logo procedures are runing
int1 gblButtonPressed = 0;    // flags when the run button is pressed
int1 gblBtn1AlreadyPressed = 0;
unsigned int16 gblWaitCounter =0;  // used for the wait cmd in Logo vm


unsigned int16 gblTimer = 0;   // This is the timer for the TIMER and RESETT commands

int gblCmdTimeOut = 0; // counter to make sure the command state is not stuck somewhere

int gblSerialBuffer[SERIAL_BUFFER_SIZE];
int gblSerialBufferPutIndex=0;
int gblSerialBufferGetIndex=0;
int gblSerialBufferIsFull=FALSE;

//////////////////////////////////////////////////////////
// I2C add-on display module variables
//////////////////////////////////////////////////////////

int gblTimeToProbeDisplayModule = 0;
int1 gblDisplayPresent = 0;
int gblTimeToSendSensorValues = 0;
int gblDisplayAddress = 0;   // the gogo will auto detect any connected display
int gblDisplayAddressList[4] = {DISPLAY_7SEG_ADDR_1, DISPLAY_7SEG_ADDR_2, DISPLAY_LCD_ADDR_1, DISPLAY_LCD_ADDR_2}; // All the possible display addresses
                                                // 0xB0 = 7-segment #1
                                                // 0xB2 = 7-segment #2
                                                // 0xB4 = LCD #1
                                                // 0xB6 = LCD #2
int1 gblAutoDetectDisplays = 1; // flag indication whether or not we should auto detect displays
                                // This flag will be off when a user explicitly addresses a 
                                // display module in the Logo code. It will be reset only after
                                // a re-boot.

int1 gblI2CisBusy = 0;

//////////////////////////////////////////////////////////
// IR variables
//////////////////////////////////////////////////////////

int16 gblIRCode = 0;       // holds the most recently recived IR Code
int16 gblIRCodeTemp = 0 ;  // work variable
int1 gblCCP2_FE = 1;       // flag to determine a Falling Edge event
int16 gblREPulseTimeStamp=0;
int gblIRPulseCount = 0;  // identifies the current pulse #
//int16 gblIRLog[13] = {0};
int1 gblIRActivity = 1;  // indicate that there has been an IR activity.
                         // Timer3's ISR uses this flag to determine when
                         // an IR transmission is complete
int16 gblPrevIR;         // records the prev IR code
int IRThreashold=0;   
int1 gblNewIRCodeHasArrivedFlag=0;  // flag used in the Logo VM

int1 gblNeedToBeep=0;   // flag telling the main loop to beep. Use to indicate
                        // when logo procedure download is done.
int1 gblPauseI2CActivity = 0;  // when true-> all display i2c activity is paused


#if defined(__PCH__)
int gblFlashBuffer[getenv("FLASH_WRITE_SIZE")]; // buffer for flash write operations
int16 gblFlashOffsetIndex=0; // where in the flash block to write 
int16 gblFlashBaseAddress; // where the flash buffer shuld be written to in the flash mem
int16 gblFlashBufferCounter = 0;  // counts the number of bytes to write
#endif

int gblCommChannel=COMM_USB;


/////////////////////////////////////////////////////////////////////////
//
//   I N T E R R U P T    S E R V I C E    R O U T I N E S
//
/////////////////////////////////////////////////////////////////////////



#int_rda
void serial_isr(void)
{

   if (gblSerialBufferIsFull == FALSE)
   {
      gblSerialBuffer[gblSerialBufferPutIndex] = getchar();         
      gblSerialBufferPutIndex++;
      if (gblSerialBufferPutIndex >= SERIAL_BUFFER_SIZE)
         gblSerialBufferPutIndex = 0;   
      
      if (gblSerialBufferPutIndex == gblSerialBufferGetIndex)
         gblSerialBufferIsFull = TRUE;
     
   }
}






#int_rtcc                          
void rtccISR() {                      


      set_rtcc(T0_COUNTER);  // this is to keep the interrupt period constant 

      gblTimer++;  // increase the global timer

      // if the board is receiving a command from a computer
      // then increase the timeout counter
      if (CMD_STATE != WAITING_FOR_FIRST_HEADER) {
         gblCmdTimeOut++;  // Command timout counter. Used at the end of main()
      }

      // the Wait opcode sets gblWaitCounter,
      // then waits until it gets to 0.
      if (gblWaitCounter > 0) { gblWaitCounter--; }


      ////////////////////////////////////////////
      // check menu button (button 1)
      // if it is pressed
      if (input(RUN_BUTTON) == 0) {
            // if button not already pressed
            if ( gblBtn1AlreadyPressed) {
               // do nothing if button press has been processed before
            } else {
               gblButtonPressed = !gblButtonPressed;
               gblBtn1AlreadyPressed=1;
               
               startStopLogoProcedures();
               
              ////////////////////////////////////////////////////////

            } 
      } else if (gblBtn1AlreadyPressed) {
         gblBtn1AlreadyPressed=0;
      }

      // tells main() to check if the display module is connected every 1 second
      gblTimeToProbeDisplayModule++;                               

}


// Timer1 contains the motor PWM signal algorithm

#int_timer1
void timer1ISR() {

   int associatedPorts;
   int i;
   
   ////////////////////////////////////////////////////////////////////////////
   // If this interrupt has taken place at the end of a PWM pulse. Thus, we need
   // to put these pulses "low" 
   ////////////////////////////////////////////////////////////////////////////
   
   if (gblPWMVectorIndexCounter < gblPWMVectorLen) {
      // cache array val in a normal variable
      associatedPorts = gblPWMVectorPorts[gblPWMVectorIndexCounter];
      for (i=0 ; i<MotorCount ; i++) {
         if (getBit(associatedPorts, i)) {
            if (getBit(gblMotorONOFF, i) == ON) {
               if (getBit(gblMotorMode, i) == MOTOR_NORMAL) {
                     setLow(MotorCWPins[i]);
                     setLow(MotorCCPins[i]);
               }
               // this is the servo mode
               else
                  setLow(MotorCCPins[i]);
            }
         }
      }
      
      ///////////////////////////////////////////////
      // setup for the next interrupt
      ///////////////////////////////////////////////
      if (++gblPWMVectorIndexCounter < gblPWMVectorLen) {
         set_timer1(gblPWMVector[gblPWMVectorIndexCounter]);
      } else {
         set_timer1(gblPWMTerminatingVector);
         gblAtEndOfPWMCycle = 1;  // notify main loop that we have reached the end
                                  // of a PWM cycle. It can make changes to the 
                                  // PWM vectors if needed (without disrupting 
                                  // the PWM pulses)
      }
      
   } 

   /////////////////////////////////////////////////////////////
   //  Else then this interrupt is at the beginning of a new PWM pulse
   //  Thus, we need to put all the pulses "high"
   /////////////////////////////////////////////////////////////


   else {
         gblAtEndOfPWMCycle = 0;

         for (i=0 ; i<MotorCount ; i++)  {
            if (getBit(gblMotorONOFF, i) == ON) {
               //if (i==0) { output_high(USER_LED); } else {output_low(USER_LED);}
               if (gblMtrDuty[i] > 0) {
                  if (getBit(gblMotorMode, i) == MOTOR_NORMAL) {
                     if (getBit(gblMotorDir, i)) {
                        setHigh(MotorCWPins[i]);
                        setLow(MotorCCPins[i]);
                     } else {
//                        if (i == 0) {
                           setHigh(MotorCCPins[i]);
                           setLow(MotorCWPins[i]);
//                        }
                     }
                  }
                  // this is the servo mode
                  else {
                     setHigh(MotorCCPins[i]);  // this is the servo pulse pin
                     setHigh(MotorCWPins[i]);  // this is the servo powor pin
                  }
               } 
               // force both CC, CW pins low if power is 0
               // this is just in case something has accidentally
               // cased any pin to go high
               else {
                  setLow(MotorCWPins[i]);
                  setLow(MotorCCPins[i]);
               }
            } else {  // turn motors off here
              setLow(MotorCWPins[i]);
              setLow(MotorCCPins[i]);
            }

            
         }
      
      
      if (gblPWMVectorLen > 0) {

         ///////////////////////////////////////////////
         // setup for the next interrupt
         //////////////////////////////////////////////
         gblPWMVectorIndexCounter = 0;
         set_timer1(gblPWMVector[0]);
      
     
      } else {  // PWM is not in use -> keep interrupt rate = PWM Period
         set_timer1(PWM_PERIOD);
         gblAtEndOfPWMCycle = 1; // needed to make this flag work when no PWM is active
      }


   }

}




// timer2 is neccessary for the PIC's hardware PWM (which drives the beeper)
#int_timer2
void timer2ISR() {
   gblBurstModeTimerHasTicked=1;  // this triggers the slow burst mode to send data. See DoSensorStuff()

   gblTimeToSendSensorValues++; // signal main() to send sensor data to the diaply module
   
   showNextDigit();
  
}

// timer3 interrupt is fired only when no IR pulses have been received (end of transmission)
#int_timer3
void timer3ISR() {
   
   // if no IR Activity in the past timer3 period -> assume IR data is done
   if (!gblIRActivity) {
      // only update if a new value has arrived
      if (gblIRCodeTemp != 0) {
         gblIRCode = gblIRCodeTemp;
         gblIRCodeTemp = 0;
         gblNewIRCodeHasArrivedFlag = 1;  // this flag is used in the Logo VM
      }

      // if time has passed with no activity -> reset gblPrevIRCode so that it
      // recognizes any incoming IR Code as a new event (even if it is the same
      // code as before).
      if (IRThreashold++ > 50) {
         gblPrevIR = 0;
         IRThreashold = 0;
      }

   } else {
      // if there is IR activity -> check for special purpose codes
      
      if (gblPrevIR != gblIRCode) {
         
         if (gblIRCode == 149 ) {   // ON/OFF button -> run/stop Logo procedures
            startStopLogoProcedures();
         }
         
         gblPrevIR = gblIRCode; // to prevent re-entrance
         IRThreashold = 0; 
      }
   }
   gblIRActivity = 0;
}


#int_ccp2
void ccp2_isr() {

//  Captures IR pulses
//
//  For a SONY remote:
//
//  Logic 0 = 0.6ms high + 0.6ms low
//  Logic 1 = 1.2ms high + 0.6ms low
//  Start Bit = 2.4ms high + 0.6ms low
//
//  Note that the IR sensor on the board inverts the logic above!
//
//  commands are transmitted every 45 ms (from start to start) when the
//  remote button is held pressed.


   int16 pulseWidth;

   gblIRActivity = 1;  // indicate that there has been an IR activity.
                       // Timer3's ISR uses this flag to determine when
                       // an IR transmission is complete

   // If a falling edge event occured, We will timestamp the CCP_2 value
   // and configure CCP2 to interrupt again at the raising edge.
   if (gblCCP2_FE) {
      
      setup_ccp2( CCP_CAPTURE_RE);
      gblCCP2_FE = 0;
      
      gblREPulseTimeStamp = CCP_2;
      
   
   } else {
      // A rising edge event occured. We calculate the pulse width to
      // determine a bit 0 or bit 1. 

      setup_ccp2( CCP_CAPTURE_FE);
      gblCCP2_FE = 1;
   
      if (CCP_2 > gblREPulseTimeStamp)  // this is the normal case 
         pulseWidth = CCP_2 - gblREPulseTimeStamp;
      else  // this means time3 has wrapped around
         pulseWidth = 65535 - gblREPulseTimeStamp + CCP_2;
      
      //gblIRLog[gblIRPulseCount] = pulseWidth;
      

      if (pulseWidth < 3000) {
         // ignore short pulses -> probably noise
         gblIRCodeTemp = 0;
         gblIRPulseCount = 0;
        
      } else if (pulseWidth < 5000) {  // Logic 0
                                       // theoretical pulse count is 3600 (0.6 ms)
                                       // actual pulses from test hovers just above 4000
         // just increase the counter
         gblIRPulseCount++;

      } else if (pulseWidth < 9000) {  // Logic 1
                                       // theoretical pulse count is 7200 (1.2 ms)
                                       // actual pulses from test hovers around 7600
      
         // set the bit 
         gblIRCodeTemp |= (1 << gblIRPulseCount);
         gblIRPulseCount++;

      }   else {
         // a long pulse is marks the beginning of a new IR transmission
         gblIRCodeTemp = 0;
         gblIRPulseCount = 0;
      }
      
   
   }



}

/*
#INT_LOWVOLT
void lowVoltISR() {
   stopLogoProcedures();
}
*/

void stopLogoProcedures(void) {

//      output_toggle(USER_LED);
//      disable_interrupts(GLOBAL);     // why do this??

      gblWaitCounter = 0;  // reset wait command (so the running Logo wait code
                           // stops immediately).
      gblONFORNeedsToFinish = 0; // incase an onfor is running.
      gblLogoIsRunning = 0;
      flgNeedToTurnOffAllMotors=1;  // tell the main loop to turn off the motors
      cls_internal7Seg();   // clear the internal 7-segment's screen

      output_low(RUN_LED);
}




void startStopLogoProcedures(void) {

      gblWaitCounter = 0;  // reset wait command (so the running Logo wait code
                           // stops immediately).
      gblONFORNeedsToFinish = 0; // incase an onfor is running.


     ////////////////////////////////////////////////////////
     // run Logo procedures
     if (!gblLogoIsRunning)
     {
         srand(gblTimer);  // seed for the random function;
         //ms1test
         //output_high(RUN_LED);
         output_high(RUN_LED);
         /////////////////////////////////////////////////
         //   get the address of the procedure
         //   to run.

         // gblMemPtr is defined in eeprom.h
//                  gblMemPtr = read_program_eeprom(RUN_BUTTON_BASE_ADDRESS) << 8;
//                  gblMemPtr += read_program_eeprom(RUN_BUTTON_BASE_ADDRESS+1);

        #if defined(__PCM__)
         gblMemPtr = read_program_eeprom(RUN_BUTTON_BASE_ADDRESS+1);
        #elif defined (__PCH__)
         //gblMemPtr = read_program_eeprom( (RUN_BUTTON_BASE_ADDRESS+1)<<1 );
         gblMemPtr = (read_program_eeprom( RUN_BUTTON_BASE_ADDRESS ) << 8) + read_program_eeprom( (RUN_BUTTON_BASE_ADDRESS+2) );
//         gblMemPtr = gblMemPtr << 1;
         
       
        #endif

         clearStack();
         gblNewByteHasArrivedFlag=0;
         gblLogoIsRunning = 1;
      } else {  // stop Logo
         stopLogoProcedures();
      }

}



void setLow(IOPointer Pin)
{
//   *(Pin>>3) &= (~(1<<(Pin&7)));
   output_bit(Pin, 0);
}

void setHigh(IOPointer Pin)
{
//   *(Pin>>3) |= (1<<(Pin&7));
   output_bit(Pin, 1);

}


int readPin(IOPointer Pin)
{
//   return (*(Pin>>3) & (1<<(Pin&7))) ;
   return (input(Pin));

}



//////////////////////////////////////////////////
short getBit(int InByte, int BitNo)
{  return ((InByte >> BitNo) & 1);
}

void setBit(int *InByte, int BitNo)
{  *InByte |= (1<<BitNo);
}

void clearBit(int *InByte, int BitNo)
{  *InByte &= ~(1<<BitNo);
}


void active_comm_putc(char c) {
   if (gblCommChannel == COMM_SERIAL) {
      putc(c);
   }else if (gblCommChannel == COMM_USB) {
      usb_cdc_putc(c);
   }
}

/////////////////////////////////////////////////////////////////////////
//
//   M O T O R   C O N T R O L
//
/////////////////////////////////////////////////////////////////////////

void TalkToMotor(int MotorBits)
{

   // Each bit represents one motor. i.e 0b00010010 means motor 2 and 5 are active
   gblActiveMotors = MotorBits;

//   printf("%c%c%c", ReplyHeader1, ReplyHeader2, ACK_BYTE);  // send acknowledge byte

}


void MotorControl(int MotorCmd)
{
   int i;

   for (i=0;i<MotorCount;i++)
   {
      if ((gblActiveMotors >> i) & 1 )
      {
         SetMotorMode(MOTOR_NORMAL);

         switch (MotorCmd)
         {
         case MTR_ON:   MotorON(i);
               break;
         case MTR_OFF: MotorOFF(i);
               break;
         case MTR_RD: MotorRD(i);
               break;
         case MTR_THISWAY: MotorThisWay(i);
               break;
         case MTR_THATWAY: MotorThatWay(i);
               break;
         case MTR_COAST: MotorCoast(i);
               break;
         }

      }
   }
   

}

/////////////////////////////////////////////////
// Refer to the problem explained in MotorControl() if
// this function suddenly missbehave
void SetMotorPower(int Power)
{
   int i;

   for (i=0;i<MotorCount;i++)
   {
      if ((gblActiveMotors >> i) & 1 )
      {

         // Starting from GoGo2.2ms5 and higher, a better implementation of
         // motor PWM has been implemented. Now, the user can
         // control up to 255 power levels (instead of 8). 
         // Note: The PWM algorithm has been improved again in GoGo 4.0 ms1
         // But to keep the
         // firmware backward compatible, the first 8 power levels are mapped
         // out evenly between 0-255.

         switch (Power) {
//            case 0: Power = 0x00; break;  // commented out because values don't change anyway
            case 1: Power = 36; break;
            case 2: Power = 73; break;
            case 3: Power = 110; break;
            case 4: Power = 146; break;
            case 5: Power = 183; break;
            case 6: Power = 219; break;
            case 7: Power = 255; break;
         }

         gblMtrDuty[i] = Power;
         if (Power != gblMtrPrevDuty[i]) {
            gblMtrNeedToRecreateMtrDutyVector = 1; // Notify the main loop that we need to
                                                   // update the duty vector
            gblMtrPrevDuty[i] = Power;
         }
      }
   }
   
}


void ChangeMotorPower(int delta) {
   int i;

   for (i=0;i<MotorCount;i++)
   {
      if ((gblActiveMotors >> i) & 1) {
         gblMtrDuty[i] = gblMtrDuty[i] + delta;
      }
   }

   createPWMVectorTable();
}



// createPWMVectorTable() will create an interrupt vector table. 
//
// The goal is for timer1 to interrupt at the particular time each pulse needs to 
// be set low. A table will be created containing the number of ticks to the next 
// interrupt. For example, here's a hypothetical situation:
//
// Motor A: PWM level = 255 (Full power)
// Motor B: PWM level = 100
// Motor C: PWM level = 50
// Motor D: PWM level = 100
//
// The vector table will look like this
//
//    int16 PWMVector[4] = [65535-(50*150), 65535-((100-50) * 150), 0, 0]
//    int PWMVectorPorts[4] = [0b0100, 0b1010, 0, 0]
//    int PWMVectorLen = 2
//    int16 PWMTerminatingVector = 65535 - (38400 - 100*150)
//
//    * The PWMVector array contains Timer1's counter value that would overflow 
//      (causing an interrupt) at the desired time
//    * The PWMVectorPorts contains bits that indicate which motor ports are 
//      associated with that power level.
//    * PWMVectorLen indicates how many vectors are present
//    * PWMTerminatingVector contains the number of ticks until then end of that 
//      PWM period (after the last pulse).
//
// Timer1 Settings
//
//    * The PWM period is 12.8ms or approximately 78Hz.
//    * This allows 256 levels at 0.05 ms pulse resolution.
//    * When Timer1 prescale is 4, the number of ticks for a 12.8ms period is 38400. 
//      Divide that by 256 pulse levels and you will get 150 ticks per level. 
//      For example, if we decide to widen a pulse by 10 steps (out of 256) we will 
//      need Timer1 to count 150x10 = 1500 ticks more before setting the pulse low.



void createPWMVectorTable(void) {

   int portsProcessed=0;  // each bit tracks which ports have been processed
   int i=0;
   int vectorIndex=0; // the current item in the PWM Vector Table
   int currentPowerLevel; // buffer to cache an array value (just to make exe faster)
   int currentMinLevel=255; // tracks the current min power level
   int portList=0; // each bit tracks the ports with the same power level
   int prevMinLevel=0; // remembers the previous min power level
   
   
   gblPWMVectorIndexCounter=0; // reset the PWM pulses

   // while not all ports have been processed
   while (portsProcessed != 0b1111) {

      for (i=0;i<MotorCount;i++) {
         
         // skip if this port has already been processed
         if (getBit(portsProcessed,i))
            continue;
         
         // cache the array value in a normal variable (it's faster)
         currentPowerLevel = gblMtrDuty[i]; 
   
         // don't create vector item for power levels 0, 255 
         if ((currentPowerLevel == 255) || (currentPowerLevel == 0)) {
            setBit(&portsProcessed,i);
            continue;
         }
   
         // look for the min power level
         if (currentPowerLevel < currentMinLevel) {
            currentMinLevel = currentPowerLevel;
            portList = 1 << i;   // remember which port this is
          
         // if found another port with equal min power level
         } else if (currentPowerLevel == currentMinLevel) {
            portList |= (1 << i);  // add the port to the list
         }   
      } 

      portsProcessed |= portList; // mark ports in the list as processed

      // if an unprocessed power level was found -> add it to the vector array

      if (currentMinLevel < 255) {      
         // We store the number of "Timer ticks" till the next timer interrupt
         gblPWMVector[vectorIndex] = 65535 - ((int16)(currentMinLevel-prevMinLevel) * 150);
         // identify the ports associated with this power level
         gblPWMVectorPorts[vectorIndex] = portList;
         
         // update working variables
         prevMinLevel = currentMinLevel;
         currentMinLevel = 255;   // reset the variable
         vectorIndex++;
      }
          
   }
   // Vector length
   gblPWMVectorLen = vectorIndex;
   
   if (vectorIndex > 0)
      // Time (ticks) from the last pulse to the end of the PWM period
      // equivalent to 65535 - (PWM_Period_Ticks - (prevMinLevel * 150))
      gblPWMTerminatingVector = PWM_PERIOD + ((int16)prevMinLevel*150);
   else
      gblPWMTerminatingVector = PWM_PERIOD;




}


// Set the mode of the active motors (NORMAL or SERVO)
void SetMotorMode(int motorMode) {
   int i;

   for (i=0;i<MotorCount;i++) {
      if ((gblActiveMotors >> i) & 1)
         if (motorMode == MOTOR_NORMAL)
            clearBit(&gblMotorMode, i);
         else // Servo mode
            setBit(&gblMotorMode, i);

   }
}


// Sets "both" the EN pins of a h-bridge chip
void ENHigh(int groupNo) {
   groupNo<<=1;

//   setHigh(MotorENPins[groupNo]);
//   setHigh(MotorENPins[groupNo+1]);



   // power on the motor chip
   if (!groupNo) {
      output_high(MOTOR_AB_EN);
   } else {
      output_high(MOTOR_CD_EN);
   }

}


// Clears "both" the EN pins of a h-bridge chip
// but do it only if both motors on the chip
// is in the coast state
void ENLow(int groupNo) {
   groupNo<<=1;

//   setLow(MotorENPins[groupNo]);
//   setLow(MotorENPins[groupNo+1]);

   // power off the motor chip
   if (!groupNo) {
      output_low(MOTOR_AB_EN);
   } else {
      output_low(MOTOR_CD_EN);
   }


}



void MotorON(int MotorNo)
{

// no need to directly output to pins here. the PWM routine in Timer1
// will take care of it asynchronously. Only update the ONOFF flags
// -Roger 30 June 2012. Firmware v13.

//!   IOPointer MtrCC, MtrCW;
//!
//!   MtrCW       = MotorCWPins[MotorNo];
//!   MtrCC       = MotorCCPins[MotorNo];
//!
//!   // if power level is 0 -> don't turn on the motor
//!   if (gblMtrDuty[MotorNo] > 0) {
//!      if (getBit(gblMotorDir,MotorNo))
//!           {    setLow(MtrCC); setHigh(MtrCW);   }
//!      else
//!         {   setHigh(MtrCC); setLow(MtrCW);   }
//!   }
   
   // In gogo board 2.3 and later, we have to set both the EN pins on the h-bridge
   // to turn on one motor port. That is
   //  Motor 1 needs EN1 and EN2 to be high
   //  Motor 2 needs the same as motor 1
   //  Motor 3 needs EN3 and EN4 to be high
   //  Motor 4 needs the same as motor 3
   //
   //  ENHigh handles this.
   ENHigh(MotorNo>>1);

   setBit(&gblMotorONOFF,MotorNo);
   
}



void MotorOFF(int MotorNo)
{

   IOPointer MtrCC, MtrCW;

   MtrCW       = MotorCWPins[MotorNo];
   MtrCC       = MotorCCPins[MotorNo];

   
   setHigh(MtrCC);
   setHigh(MtrCW);

   clearBit(&gblMotorONOFF,MotorNo);
   ENHigh(MotorNo>>1);     // this ensures the motor breaks. Sometimes the timer_isr turns off the enable pin, which turns 'stop' into 'coast'

}



void MotorRD(int MotorNo)
{
// no need to directly output to pins here. the PWM routine in Timer1
// will take care of it asynchronously. Only update the Motor direction flags
// -Roger 30 June 2012. Firmware v13.

//!   IOPointer MtrCC, MtrCW;
//!
//!    MtrCW       = MotorCWPins[MotorNo];
//!    MtrCC       = MotorCCPins[MotorNo];
//!
//!      if (getBit(gblMotorDir,MotorNo))
//!      {   if (getBit(gblMotorONOFF, MotorNo)) {
//!            setLow(MtrCW);
//!            setHigh(MtrCC);
//!         }
//!         clearBit(&gblMotorDir,MotorNo);
//!      } else
//!      {   if (getBit(gblMotorONOFF, MotorNo)) {
//!            setHigh(MtrCW);
//!            setLow(MtrCC);
//!         }
//!         setBit(&gblMotorDir,MotorNo);
//!      }
//!

      if (getBit(gblMotorDir,MotorNo))
      {   clearBit(&gblMotorDir,MotorNo);
      } else
      {   setBit(&gblMotorDir,MotorNo);
      }


}


///////////////////////////////////////////////////////////////////


void MotorThisWay(int MotorNo)
{   
// no need to directly output to pins here. the PWM routine in Timer1
// will take care of it asynchronously. Only update the Motor direction flags
// -Roger 30 June 2012. Firmware v13.

//!   IOPointer MtrCC, MtrCW;
//!
//!     MtrCW       = MotorCWPins[MotorNo];
//!     MtrCC       = MotorCCPins[MotorNo];
//!
//!     setBit(&gblMotorDir,MotorNo);
//!
//!      if (getBit(gblMotorONOFF, MotorNo)) {
//!          setLow(MtrCC);
//!            setHigh(MtrCW);
//!      }
//!
     setBit(&gblMotorDir,MotorNo);


}


void MotorThatWay(int MotorNo)
{   
// no need to directly output to pins here. the PWM routine in Timer1
// will take care of it asynchronously. Only update the Motor direction flags
// -Roger 30 June 2012. Firmware v13.

//!   IOPointer MtrCC, MtrCW;
//!
//!     MtrCW       = MotorCWPins[MotorNo];
//!     MtrCC       = MotorCCPins[MotorNo];
//!
//!     clearBit(&gblMotorDir,MotorNo);
//!
//!      if (getBit(gblMotorONOFF, MotorNo)) {
//!         setLow(MtrCW);
//!           setHigh(MtrCC);
//!      }
//!
   clearBit(&gblMotorDir,MotorNo);

}


void MotorCoast(int MotorNo)
{
// no need to directly output to pins here. the PWM routine in Timer1
// will take care of it asynchronously. Only update the ONOFF flags
// -Roger 30 June 2012. Firmware v13.

//!   IOPointer MtrCC, MtrCW;
//!
//!     MtrCW       = MotorCWPins[MotorNo];
//!     MtrCC       = MotorCCPins[MotorNo];
//!
//!        clearBit(&gblMotorONOFF,MotorNo);
//!
//!      setLow(MtrCW);
//!      setLow(MtrCC);
//!

   clearBit(&gblMotorONOFF,MotorNo);


   // if both ports on the h-bridge is off then turn off the
   // EN pins
   if (  !((gblMotorONOFF >> (MotorNo & 0b10)) & 1) &&
         !((gblMotorONOFF >> ((MotorNo & 0b10) + 1)) & 1) ) {
            ENLow(MotorNo>>1);
         }
}

void DoMotorStuff() {
   
   // Update the motor duty vector if needed.
   // Do this only at the end of a PWM cycle so that we don't disrupt
   // the current PWM generation process.
   
   if (gblAtEndOfPWMCycle) {
      if (gblMtrNeedToRecreateMtrDutyVector) {
         createPWMVectorTable();
         gblMtrNeedToRecreateMtrDutyVector = 0;

      }
   }

}



///////////////////////////////////////////////////////////
//
//    M i s c   C o n t r o l
//
///////////////////////////////////////////////////////////

void miscControl(int cur_param, int cur_ext, int cur_ext_byte)
{
//   int16 counter;

   switch (cur_param) {
      case MISC_USER_LED:
         if (cur_ext == TURN_USER_LED_ON) { USER_LED_ON; }
         else {USER_LED_OFF; }
         break;
      case MISC_BEEP:
         beep();
         break;
      case MISC_SET_PWM:
         MotorControl(MTR_ON);
         MotorControl(MTR_THISWAY);
         SetMotorMode(MOTOR_SERVO);
         SetMotorPower(cur_ext_byte);
         break;

// we handle EEPROM upload in the main loop (urr .. ugly code)
      case MISC_UPLOAD_EEPROM:

         break;

      case MISC_I2C_SETUP:
         switch (cur_ext) {
            case I2C_START:
               gblI2CisBusy = 1;  // reserve the i2c bus
               i2c_start();
               break;
            case I2C_STOP:
               i2c_stop();
               gblI2CisBusy = 0;  // release the i2c bus
               break;
            case I2C_WRITE:
               i2c_write(cur_ext_byte);
//               printf("%c", cur_ext_byte);
               break;
            case I2C_READ:
               i2c_read(0);
               break;
            }
         break;

   case MISC_AUTORUN_CONFIG:
      switch(cur_ext_byte) {
         case AUTORUN_ON:
             FLASHSetWordAddress(AUTORUN_STATUS_ADDRESS);
             FLASHWrite(AUTORUN_ON);
             break;
         case AUTORUN_OFF:
             FLASHSetWordAddress(AUTORUN_STATUS_ADDRESS);
             FLASHWrite(AUTORUN_OFF);
             break;
      }
      break;
   }
}


/////////////////////////
//    User LED controls

void uLED_on()
{  output_high(USER_LED);
}

void uLED_off()
{  output_low(USER_LED);
}


///////////////////////
//   Beep

void beep() {
    set_pwm1_duty(50);        // make a beep
    delay_ms(50);
    set_pwm1_duty(0);        // make a beep
}




/////////////////////////////////////////////////////////////////////////
//
//   P R O C E S S   S E N S O R    W H I L E    I D L E
//
/////////////////////////////////////////////////////////////////////////



void DoSensorStuff()
{
//   int i;
   long SensorVal;

   //////////////////////////////////////////////////////////////////
   //
   //  Burst sensor data to computer

   if (gblBurstModeTimerHasTicked) {


      // if this sensor is in Burst mode -> send value to computer
      if ((gblBurstModeBits>>gblBurstModeCounter) & 1)
      {
          SensorVal=readSensor(gblSensorPortMap[gblBurstModeCounter]);
          printf(active_comm_putc, "%c%c%c", 0x0c, (gblBurstModeCounter << 5) | (SensorVal >> 8), SensorVal & 0xff);
      }

      // gblBurstModeCounter will circulate from 0 - 7
      gblBurstModeCounter = (gblBurstModeCounter+1) % 8;
      
      gblBurstModeTimerHasTicked = 0;
    }




}


/////////////////////////////////////////////////////////////////////////
//
//   S E N S O R   C O N T R O L
//
/////////////////////////////////////////////////////////////////////////


void SetBurstMode(int SensorBits, int Mode)
{

   gblBurstModeBits = SensorBits;
   
   // As of gogo 4.0 there are no more slow/fast modes. 
   // Setting this here has no effect.
   if (Mode > 0)
      gblSlowBurstMode = 1; // switch to SLOW burst mode.
   else
      gblSlowBurstMode = 0; // switch to normal (fast) burst mode.


}


unsigned int16 readSensor(int sensorNo) {
      if (gblCurSensorChannel != sensorNo)
      {   switchAdcChannel(sensorNo);
           gblCurSensorChannel=sensorNo;
      }
      return(getSensorVal());
}



/////////////////////////////////////////////////
long getSensorVal()
{
   long sensorVal;

   delay_us(channelSwitchDelay);   // wait for adc to stabilize
                                   // Although this so often unneeded and can
                                   // slow down the execution, we are playing
                                   // it safe
   sensorVal=read_adc();
   //return (sensorVal >> 6);  // use this line if using PIC-C compiler version 2.x

   // the PIC 16F77 ADC is only 8 bits. So, we simulate 10 bits by shifting
   // left 2 bits. But why in the code we shif right 6 bits? This is because
   // somehow the 8 bit sensor readings are stored in the higher byte
   // of the long variable. So, we have to shift right 8 bits before shifting
   // left. So, 8-2 is 6.
   #IFDEF PIC16F77
   sensorVal >>= 6;
   #ENDIF

   return (sensorVal);        // this line works with PIC-C compiler version 3.x

}

/////////////////////////////////////////////////
void switchAdcChannel(int channelNo) {
   set_adc_channel(channelNo);
}


//////////////////////////////////////////////////////////////////////////////
// 
// Fetch a character from the serial buffer
//
//////////////////////////////////////////////////////////////////////////////

byte readSerialBuffer(byte *charPtr)
{
   int errorCode;
   
   if (gblSerialBufferIsFull == TRUE)
   {
      gblSerialBufferIsFull = FALSE;
      errorCode = SERIAL_OVERFLOW;   
      gblSerialBufferPutIndex = 0;
      gblSerialBufferGetIndex = 0;
      *charPtr = 0;      
   }
   else if (gblSerialBufferGetIndex == gblSerialBufferPutIndex)   
   {
       errorCode = SERIAL_NO_DATA;      
       *charPtr = 0;
   }
   else
   {
      *charPtr = gblSerialBuffer[gblSerialBufferGetIndex];
      gblSerialBufferGetIndex++;
      if (gblSerialBufferGetIndex >= SERIAL_BUFFER_SIZE)
      {  gblSerialBufferGetIndex = 0;
      }
      errorCode = SERIAL_SUCCESS;   
   }
   return(errorCode);
}


int1 serialKbhit() {
   return(gblSerialBufferPutIndex != gblSerialBufferGetIndex);
}

char serialGetChar() {
   char foo;
   
   readSerialBuffer(&foo);
   return(foo);
   
}


/////////////////////////////////////////////////////////////////////////
//
//   A D D - O N   M O D U L E S 
//
/////////////////////////////////////////////////////////////////////////



void DoDisplayModuleStuff(void) {

   // if i2c is being used
   if (gblI2CisBusy)
      return;
      
   // if logo code is being downloaded
   if (gblPauseI2CActivity)
      return;

   // Auto detect a display module
   // gblTimeToProbeDisplayModule increases every 0.1 sec in timer1
   if ((gblTimeToProbeDisplayModule > 10) && (gblAutoDetectDisplays)) {
      gblDisplayAddress = displayPing();
      gblDisplayPresent = gblDisplayAddress? 1:0 ;  // gblDisplayPresent=1 if a display is found (gblDisplayAddress !=0)
      gblTimeToProbeDisplayModule = 0;
   }
   
   
   // Send sensor values to the display module
   // True if a display module is present or when manual mode is activated from the Logo code
   // gblTimeToSendSensorValues is set in Timer2 
   if ((gblTimeToSendSensorValues > 3) && (gblDisplayPresent || !gblAutoDetectDisplays)) {
      displaySendSensors();
      gblTimeToSendSensorValues = 0;
   }
}   


/////////////////////////////////////////////////////////////////////////
//
//   M I S C
//
/////////////////////////////////////////////////////////////////////////

void init_variables()
{  
   gblBurstModeBits = 0;

   CMD_STATE = WAITING_FOR_FIRST_HEADER;


   ///////////////////////////////////
   // Logo VM variables
   gblLogoIsRunning=0;         // if set, Lovo VM will start fetching opcode
                     // from EEPROM and run them.
   gblStkPtr=0;      // Stack pointer
   gblInputStkPtr=0; // Procedure input stack pointer
   gblErrFlag=0;     // Error flag. Not really used.



   // init the record pointer to the one last saved in the EEPROM
   // see RECORD in evalopcode.c to see how this value is logged
   #if defined(__PCM__)
   gblRecordPtr = read_ext_eeprom(MEM_PTR_LOG_BASE_ADDRESS) + (read_ext_eeprom(MEM_PTR_LOG_BASE_ADDRESS+1) << 8);
   #elif defined(__PCH__)
   gblRecordPtr = read_program_eeprom(MEM_PTR_LOG_BASE_ADDRESS);
   
   #endif
}


void intro ()
{

    set_pwm1_duty(50);        // make a beep
    output_high(RUN_LED);
    delay_ms(50);
    set_pwm1_duty(0);         // stop the beep
    delay_ms(50);
    output_low(RUN_LED);

    set_pwm1_duty(50);        // make a beep
    USER_LED_ON; 
    delay_ms(50);
    set_pwm1_duty(0);         // stop the beep
    delay_ms(50);
    USER_LED_OFF; 


}





void Halt()
{       while (1) {
          output_high(RUN_LED);
          delay_ms(50);
          output_low(RUN_LED);
          delay_ms(500);
       }

}


void clearMotors() {

   int i,j;

   // Disable both motor chips
   output_low(MOTOR_AB_EN);
   output_low(MOTOR_CD_EN);


   // Init all motors to the coast state
   for (i=0,j=0 ; i<MotorCount ; i++)
   {  // setLow(MotorENPins[i]);       // Mtr Enable pin
      setLow(MotorCWPins[i]);   // Mtr CW pin.
      setLow(MotorCCPins[i]);      // Mtr CCW pin
   }
   
}


void initBoard()
{
   int i,j;



   gblActiveMotors = 0;

   set_tris_a(PIC_TRIS_A);
   set_tris_b(PIC_TRIS_B);
   set_tris_c(PIC_TRIS_C);
   set_tris_d(PIC_TRIS_D);
   set_tris_e(PIC_TRIS_E);
   set_tris_f(PIC_TRIS_F);
   set_tris_g(PIC_TRIS_G);


   // Init the analog ports
   setup_adc_ports(sAN0|sAN1|sAN2|sAN3|sAN4|sAN7|sAN10|sAN11);   // use all the 8 ADC ports. Pin names are not continguous on the 18F66J50
   setup_adc( ADC_CLOCK_DIV_64 );

   // Init default sensor port
   gblCurSensorChannel=defaultPort;
   set_adc_channel( defaultPort );


   clearMotors();


   // init the serial port
   //ser_init();

      // set RTCC to interrupt every 100ms
      setup_timer_0( RTCC_INTERNAL| RTCC_DIV_128 );
      set_rtcc(T0_COUNTER);
      enable_interrupts(INT_RTCC);

      setup_timer_1 ( T1_INTERNAL | T1_DIV_BY_4 ); // int every (65536-T1_COUNTER) * 4 * 4 * 1/48 uSec
      enable_interrupts(INT_TIMER1);
      set_timer1(0);
                                                    
      //setup_timer_2(T2_DIV_BY_16,250,16);
      setup_timer_2(T2_DIV_BY_16,250,8);
      enable_interrupts(INT_TIMER2);
        
      setup_timer_3( T3_INTERNAL | T3_DIV_BY_2 | T3_CCP2_TO_5); // T3 is used for the IR Capture. 0.6ms = 3600 counts (a SONY remote)
      set_timer3(0);
      enable_interrupts(INT_TIMER3);
      
      setup_ccp2( CCP_CAPTURE_FE ); // CCP2 is used for the IR receiver. Init to captuer a falling edge.
      enable_interrupts(INT_CCP2);      

      setup_ccp1(CCP_PWM);   // Configure CCP1 as a PWM (for the beeper)


//      setup_low_volt_detect( LVD_TRIGGER_BELOW | LVD_42 );
//      enable_interrupts(INT_LOWVOLT);

      enable_interrupts(INT_RDA);

      enable_interrupts(GLOBAL);


   /////////////////////////////////////////////////
   //
   //  Logo VM stuff
   //
      init_ext_eeprom();


    intro();

    delay_ms(500); // this dealy is to give the computer software time to
                     // kill any existing serial session in the case that this
                     // boot is a reset.


   // Initialize the USB device but does not stalls. 
   // usb_task() must be called periodically to check
   // when a computer is connected. See usb.h for more info
   usb_init_cs();


}

#if defined(__PCH__)

///////////////////////////////////////////////////////////////////////////////
//
//   F L A S H   M E M O R Y   R O U T I N E S
//
///////////////////////////////////////////////////////////////////////////////


void FLASHSetWordAddress(int16 address) {

      // calculate the address of the beginning of the current flash block
      gblFlashBaseAddress = address;
      gblFlashBaseAddress &= ((getenv("FLASH_ERASE_SIZE")-1) ^ 0xFFFF); // ^ => XOR
      
      // calculate the position within the current flash block
      gblFlashOffsetIndex = address - gblFlashBaseAddress;
      //gblFlashOffsetIndex <<= 1; // ptr points to each byte in the buffer (not word)
      
      gblFlashBufferCounter = 0;  // reset the buffer index
      

}

void FLASHBufferedWrite(int16 InByte) {
      
      gblFlashBuffer[gblFlashBufferCounter++] = (int)InByte;
      gblFlashBuffer[gblFlashBufferCounter++] = (int)(InByte>>8);  // each readable flash block is 2 bytes (we use only one here).
                    
      // when the buffer contains FLASH_WRITE_SIZE bytes -> write to the actual flash memory
      if (!(gblFlashBufferCounter < getenv("FLASH_WRITE_SIZE") )) {
      
         FLASHFlushBuffer();         
      }
}

// Write directely to the Flash without buffering. This will be slow.

void FLASHWrite(int16 InByte) {
      
      FLASHBufferedWrite(InByte);
      FLASHFlushBuffer();

}


void FLASHFlushBuffer() {

      if (gblFlashBufferCounter > 0) {
         writeFLASH(gblFlashBaseAddress, gblFlashOffsetIndex, gblFlashBufferCounter, gblFlashBuffer);

         FLASHSetWordAddress(gblFlashBaseAddress + gblFlashOffsetIndex + gblFlashBufferCounter );

      }
}


// Writes data to flash memory using block mode
// On PIC16F887, Write Size = 8 Words (16 Bytes), Erase size = 16 Words (32 Bytes)
// On PIC18F458, Write Size = 8 Bytes, Erase size = 64 Bytes
// - Whenever we write to the beginning of an erase block, the entire block (16 words) will be
//   automatically erased (set to 0x3FFF). 
// - So, whenever we want to write some bytes to the flash memory, we need to read the entire
//   erase block into a buffer, modify that buffer, then write it back to the flash memory.



void writeFLASH(int16 memoryBlockAddress, int16 positionInMemoryBlock, int16 len, int *Buffer) {

   int writeBuffer[getenv("FLASH_ERASE_SIZE")];
   int16 i, counter;
   int16 writeLenInThisBlock;
   int1 notDone = 1;

   // prevent flash write to the firmware area
   if (memoryBlockAddress < RECORD_BASE_ADDRESS) {
      while (1) { beep(); delay_ms(1000); }  
   }


   do {
   
      // read the entire erase block to memory
      read_program_memory(memoryBlockAddress, writeBuffer, getenv("FLASH_ERASE_SIZE"));
      usb_task();
      
      // if write len is longer than the current memory block -> trim it
      if ( len > (getenv("FLASH_ERASE_SIZE") - positionInMemoryBlock)) {
         writeLenInThisBlock = getenv("FLASH_ERASE_SIZE") - positionInMemoryBlock;
      } else {
         writeLenInThisBlock = len;
      }
      
      // modify parts of the block with the new data
      for (i=positionInMemoryBlock, counter=0; counter<writeLenInThisBlock; i++, counter++) {
         writeBuffer[i] = *(Buffer+counter);
      }
      usb_task();
      
      // write the block back to the flash memory. 
      for (i=0 ; i< getenv("FLASH_ERASE_SIZE") / getenv("FLASH_WRITE_SIZE"); i++) {
         write_program_memory( memoryBlockAddress + i * getenv("FLASH_WRITE_SIZE") ,
                               writeBuffer + i * getenv("FLASH_WRITE_SIZE"), 
                               getenv("FLASH_WRITE_SIZE"));
         usb_task();
      }
      
      // if write-data overlaps between memory blocks -> update variables and
      // loop to write the next block
      if ((positionInMemoryBlock + len) > getenv("FLASH_ERASE_SIZE")) {
         memoryBlockAddress += getenv("FLASH_ERASE_SIZE");
         len -= getenv("FLASH_ERASE_SIZE") - positionInMemoryBlock;
         Buffer += getenv("FLASH_ERASE_SIZE") - positionInMemoryBlock;
         positionInMemoryBlock = 0;

      } else {
         notDone = 0;
      }
      usb_task();
   
   } while (notDone);
}


#endif



/////////////////////////////////////////////////////////////////////////
//
//   S E R I A L    D A T A    H A N D L E R
//
/////////////////////////////////////////////////////////////////////////


void ProcessInput()
{   byte InByte;
   int1 doNotStopRunningProcedure=1;   // if set means we've got a Logo cmd. We need to echo back.
   int i;
   
   
   //while ((buff_status = readSerialBuffer(&InByte)) == SERIAL_SUCCESS)  // fetch character
   while ( usb_cdc_kbhit() || serialKbhit()) 
   {        
           
            if (serialKbhit()) {
               gblCommChannel = COMM_SERIAL; 
               InByte = serialGetChar();
               output_toggle(USER_LED);
            
            } else if (usb_cdc_kbhit()) {
               gblCommChannel = COMM_USB;
               InByte = usb_cdc_getc();
            }
            
            // reset the command timeout counter
            gblCmdTimeOut = 0 ;

            // this is for the SERIAL? command in the Logo VM
            // No matter what the input bytes are for, we
            // make it available to the Logo VM.
            gblMostRecentlyReceivedByte = InByte;
            gblNewByteHasArrivedFlag = 1;


            // always send an echo byte back to the PC.
            printf(active_comm_putc, "%c",InByte);


            switch (CMD_STATE) {


            ///////////////////////////////////////////////////////
            //
            //  if the board is idle, we need to distinguish which
            //  mode we are running, GoGo or Cricket Logo
            case WAITING_FOR_FIRST_HEADER:
                  switch (InByte) {
                    ////////////////////////////////////
                    // this is a GoGo header
                    case InHeader1:
                          CMD_STATE = WAITING_FOR_SECOND_HEADER;
                          //doNotStopRunningProcedure = 1;
                          break;
                    ////////////////////////////////////
                    // the rest are Cricket Logo headers
                    case SET_PTR:   // set-pointer
                          CMD_STATE = SET_PTR_HI_BYTE;
                          doNotStopRunningProcedure=0;
                          output_toggle(USER_LED);
                          break;
                    case READ_BYTES:   // read-bytes
                          CMD_STATE = READ_BYTES_COUNT_HI;
                          doNotStopRunningProcedure=0;
                          break;
                    case WRITE_BYTES:   // write-bytes
                          CMD_STATE = WRITE_BYTES_COUNT_HI;
                          doNotStopRunningProcedure=0;
                          break;
                    case RUN:   // run
                          //doNotStopRunningProcedure = 1;

                          ///////////////////////////////////
                          // start runing logo
                          output_high(RUN_LED);
                          gblWaitCounter = 0;  // reset wait command (so any running Logo wait code
                                               // stops immediately).
                          gblONFORNeedsToFinish = 0; // in case an onfor is running.
                          clearStack();
                          gblNewByteHasArrivedFlag = 0;
                          gblLogoIsRunning = 1;
                          ////////////////////////////////////

                          break;
                    case CRICKET_CHECK:  // cricket-check
                          CMD_STATE = CRICKET_NAME;
                          doNotStopRunningProcedure=0;
                          break;
                  };

                    // Stop any running logo procedures unless we got a "RUN" command
                    // or if we got a thethered gogo command
                    // which will set doNotStopRunningProcedure to 1
                    if (!doNotStopRunningProcedure) {
                        stopLogoProcedures();                            
                        doNotStopRunningProcedure=1;
                    }
                    

                 break;

            /////////////////////////////////////////////////////////////////
            //
            //   These are GoGo states
            //
            case WAITING_FOR_SECOND_HEADER:
                if (InByte == InHeader2) CMD_STATE = WAITING_FOR_CMD_BYTE;
                break;
            case WAITING_FOR_CMD_BYTE:
                 gbl_cur_cmd    =(InByte & 0b11100000) >> 5; // bit 5,6,7 are the command bits
                 gbl_cur_param   =(InByte & 0b00011100) >> 2; // bit 2,3,4 are parameter bits for motors and sensors.
                 gbl_cur_ext      =(InByte & 0b00000011);

               gblExtCmdMode = 0;  // Turn extended command mode off by default
               // if cmd byte is 0xff -> we switch to extended command mode
               if (InByte == 0b11111111) {
                   CMD_STATE = WAITING_FOR_EXT_CMD_LENGTH;
                   gblExtCmdMode = 1; // flag an extended command mode
               } else if (gbl_cur_cmd > ONE_BYTE_CMD)
               {   CMD_STATE = WAITING_FOR_SECOND_CMD_BYTE;
               } else
               {   CMD_STATE = CMD_READY;
               }
               break;
            case WAITING_FOR_SECOND_CMD_BYTE:

                gbl_cur_ext_byte = InByte;
                CMD_STATE = CMD_READY;
                break;
            case CMD_READY:
              
               break;

         
            /////////////////////////////////////////////////////////////////
            //
            //   These are GoGo Extended Command States
            //
            //////////////////////////////////////////////////////////////
         
            case WAITING_FOR_EXT_CMD_LENGTH:
                gblExtCmdLen = InByte;
                gblExtCmdBufferIndex = 0;
                CMD_STATE = WAITING_FOR_EXT_CMD_STREAM;
               break;

            case WAITING_FOR_EXT_CMD_STREAM:
               
               if (gblExtCmdBufferIndex == 0) {
                  gbl_cur_cmd    = InByte ;
               } else {
                  gblExtCMDBuffer[gblExtCmdBufferIndex-1] = InByte;
               }
               
               gblExtCmdBufferIndex++;
               
               // if we have received all the command bytes -> finish
               if (gblExtCmdBufferIndex == gblExtCmdLen) {
                  CMD_STATE = CMD_READY;
               }
            
               break;


            /////////////////////////////////////////////////////////////////
            //
            //   These are Cricket Logo states
            //
            //////////////////////////////////////////////////////////////

            //  set-pointer
            case SET_PTR_HI_BYTE:  /* get addr high byte */
               // When download is in progress -> we do not allow i2c activity
               // as it can interfere and cause problems.
               gblPauseI2CActivity = 1;
               
               gblMemPtr = (unsigned int16) InByte << 8;
               CMD_STATE = SET_PTR_LOW_BYTE;
//                     printf("%c",CMD_STATE);
               break;
            case SET_PTR_LOW_BYTE:  /* get addr low byte */

               gblMemPtr = gblMemPtr | InByte;
               CMD_STATE = WAITING_FOR_FIRST_HEADER;

               gblMemPtr = gblMemPtr << 1;    // change from word addres to byte address

               // Since gogo stores the run button vector at a different memory location,
               // we must map it correctly.
               // so, any thing between 0xff0-0xfff will be mapped to the GoGo's address

               // True when gblMemPtr is between 0x1FE0 -> 0x1FFF
               if ((gblMemPtr>>5) == 0xff) {

                  gblMemPtr = (RUN_BUTTON_BASE_ADDRESS + (gblMemPtr & 0xf)) - FLASH_USER_PROGRAM_BASE_ADDRESS ;

                  //printf(usb_cdc_putc, "btn ptr = %Lu\r\n", gblMemPtr);

                  // Logo code download almost complete.
                  // Tells the main loop to beep.  Note. We don't beep here directly because beep() includes a
                  // delay, which could cause trouble
                  output_high(USER_LED);
                  gblNeedToBeep = 1;
                  gblPauseI2CActivity = 0;
                  //cls_internal7Seg();

               }

               
         

               #if defined(__PCH__) 
               
               FLASHSetWordAddress((FLASH_USER_PROGRAM_BASE_ADDRESS) + gblMemPtr);
               
              
               #endif
               
               
               break;
               

            //////////////////////////////////////////////////////////////
            // read-bytes
            case READ_BYTES_COUNT_HI:  /* get count high byte */
               gblRWCount = (unsigned int16) InByte << 8;
               CMD_STATE = READ_BYTES_COUNT_LOW;
               break;
            case READ_BYTES_COUNT_LOW:  /* get count low byte */
               gblRWCount = gblRWCount | InByte;
               sendBytes(gblMemPtr, gblRWCount);
               gblMemPtr += gblRWCount;  // update the gblMemPtr

               CMD_STATE = WAITING_FOR_FIRST_HEADER;
               break;

            //////////////////////////////////////////////////////////////
            // write-bytes
            case WRITE_BYTES_COUNT_HI:  /* get count high byte */
               gblRWCount = (unsigned int16) InByte << 8;
               CMD_STATE = WRITE_BYTES_COUNT_LOW;
               break;
            case WRITE_BYTES_COUNT_LOW:  /* get count low byte */
               gblRWCount = gblRWCount | InByte;
               CMD_STATE = WRITE_BYTES_SENDING;

            
            
//            #if defined(__PCH__)
//               // if downloading a Logo program -> must erase all the flash memory blocks used to strore the Logo programs 
//               if (gblMemPtr == 0) {
//                  for (memoryBlockAddress=FLASH_USER_PROGRAM_BASE_ADDRESS ; memoryBlockAddress < FLASH_BOOTLOADER_BASE_ADDRESS ; memoryBlockAddress += (getenv("FLASH_ERASE_SIZE")>>1)) {
//                     erase_program_eeprom(memoryBlockAddress<<1 );
//                  }
//               }
//            #endif

               break;
            case WRITE_BYTES_SENDING:

            // 14 bit parts (16F877) writes each memory word one by one
            #if defined(__PCM__)
               write_program_eeprom(FLASH_USER_PROGRAM_BASE_ADDRESS + gblMemPtr++, (unsigned int16)InByte);
            
            // 16 bit parts (18F458) buffers the data and writes blocks of data to the falsh
            #elif defined(__PCH__)
               // write to the flash buffer (it automatically writes to the flash
               // every time the buffer is full
               FLASHBufferedWrite(InByte);
               
            #endif
               

               if (--gblRWCount < 1) { 

                  #if defined(__PCH__)
                     // write remaining data in the buffer to the flash
                     FLASHFlushBuffer();
                  #endif

                  CMD_STATE = WAITING_FOR_FIRST_HEADER;
               }

               printf(active_comm_putc,"%c",255-InByte);
               
               
               break;

            //////////////////////////////////////////////////////////////
            // cricket-check. Assumes we only ping all crickets (InByte = 0)
            // Named crickets are not supported.
            case CRICKET_NAME:
               printf(active_comm_putc,"%c",0x37);
               CMD_STATE = WAITING_FOR_FIRST_HEADER;
               break;



            default:
                CMD_STATE = WAITING_FOR_FIRST_HEADER;
               break;
         }
         if (CMD_STATE == CMD_READY)  break;


   }

    



   // else assume no data in serial buffer
}



/////////////////////////////////////////////////////////////////////////
//
//   M A I N
//
/////////////////////////////////////////////////////////////////////////


void main() {


int16 SensorVal;
int16 uploadLen,counter;


int16 foo;

int i;

   RBPU = 0;   // enable port b internal pull-up.
   PLLEN=TRUE;  // enable the PLL 
   delay_ms(1);

if (input(RUN_BUTTON) == 0) {

   set_tris_a(PIC_TRIS_A);
   set_tris_b(PIC_TRIS_B);
   set_tris_c(PIC_TRIS_C);
   set_tris_d(PIC_TRIS_D);
   set_tris_e(PIC_TRIS_E);

   output_high(USER_LED);
   output_high(RUN_LED);
   output_low(PIEZO); // make sure the buzzer is silent

   downloadBootloader();
}


init_variables();
initBoard();

//!while (1) {
//!   printf("Hello\r\n");
//!   delay_ms(500);
//!   output_toggle(USER_LED);
//!
//!}

//  If autorun is set -> run logo procedures
if (read_program_eeprom(AUTORUN_STATUS_ADDRESS)==AUTORUN_ON) {
   startStopLogoProcedures();      
}



while (1) {


   ProcessInput();
    

   if ((CMD_STATE == CMD_READY) && (gblExtCmdMode==0))  {


         switch (gbl_cur_cmd)  {
            case CMD_PING:
                  // do nothing, just send back ACK
                  break;

            case CMD_READ_SENSOR:
                  // if ext bits = 0b11 -> it is a read IR command
                  if (gbl_cur_ext == 0b11){
                     SensorVal = gblIRCode;
                     gblIRCode = 0;   // reset the IR Code
                  } else {
                     // the sensor number is mapped to the actual analog
                     // port on the processor
                     SensorVal = readSensor(gblSensorPortMap[gbl_cur_param]);
                  }
                  break;

            case CMD_MOTOR_CONTROL:
                  MotorControl(gbl_cur_param);
                  break;

            case CMD_MOTOR_POWER:
                  SetMotorPower(gbl_cur_param);
                  break;

            case CMD_TALK_TO_MOTOR:
                  TalkToMotor(gbl_cur_ext_byte);
                  break;

            case CMD_BURST_MODE:
                  SetBurstMode(gbl_cur_ext_byte, gbl_cur_ext);
                  break;

            case CMD_MISC_CONTROL:
                  miscControl(gbl_cur_param, gbl_cur_ext, gbl_cur_ext_byte);
                  break;
            }


      
      // send the appropriate response back to the computer
      if (gbl_cur_cmd == CMD_READ_SENSOR)
         printf(active_comm_putc,"%c%c%c%c",ReplyHeader1, ReplyHeader2, SensorVal >> 8, SensorVal & 0xff);
      else {
         printf(active_comm_putc,"%c%c%c", ReplyHeader1, ReplyHeader2, ACK_BYTE);  // send acknowledge byte
         if (gbl_cur_cmd == CMD_PING)  // if PING -> also send firmware version 
            printf(active_comm_putc, "%c%c%c", HARDWARE_ID1, HARDWARE_ID2, FIRMWARE_ID);
      }
         

      ///////////////////////////////////////////////////////////
      //
      //  Handle EEPROM uploads here (urrr ... ugly coding)

      /// cur_ext (2 bits) and cur_ext_byte (8 bits) combinds to form a 10 bit counter.
      /// But to cover all the 32K EEPROM address range we must multiply this count by 32 (shift left 5 bits)
      /// Therefore, one unit of the cur_ext|cur_ext_byte represents 32 EEPROM bytes (16 sensor values)
      /// This basically means we have 16 values increments.

      if ((gbl_cur_cmd == CMD_MISC_CONTROL) && (gbl_cur_param == MISC_UPLOAD_EEPROM)) {


         uploadLen = ((((int16)gbl_cur_ext<<8) + gbl_cur_ext_byte)<<5);  // number of bytes to upload

         #if defined(__PCM__)
         

         // if uploadlen is 0 -> use automatic upload. In this case upload len becomes the last EEPROM location
         // referenced by the 'record' command. This value is stored in the 'internal' EEPROM of
         // the PIC.
         if (uploadLen == 0) {
            uploadLen = read_ext_eeprom(MEM_PTR_LOG_BASE_ADDRESS) + ((int16)read_ext_eeprom(MEM_PTR_LOG_BASE_ADDRESS+1) << 8);
         }

         // send EEPROM upload header bytes + number of bytes to send
         printf(active_comm_putc, "%c%c%c%c", EEPROMuploadHeader1, EEPROMuploadHeader2, uploadLen & 0xff, uploadLen >> 8);

         for (counter = 0 ; counter < uploadLen ; counter++ )
            printf(active_comm_putc, "%c", read_ext_eeprom(RECORD_BASE_ADDRESS + counter));
      

         #elif defined(__PCH__)
      
         // if uploadlen is 0 -> use automatic upload. In this case upload len becomes the last EEPROM location
         // referenced by the 'record' command. This value is stored in the 'internal' EEPROM of
         // the PIC.
         if (uploadLen == 0) {
            uploadLen = read_program_eeprom(MEM_PTR_LOG_BASE_ADDRESS);
         }

         // number of bytes to send to the PC is twice the number of words to upload
         uploadLen<<=1;
         
         // send EEPROM upload header bytes + number of bytes to send
         printf(active_comm_putc, "%c%c%c%c", EEPROMuploadHeader1, EEPROMuploadHeader2, uploadLen & 0xff, uploadLen >> 8);
         
         // shift back becuase the nubmer of words read is half the number of bytes
         uploadLen>>=1;
         
         for (counter = 0 ; counter < uploadLen ; counter++ ) {
            foo = read_program_eeprom((RECORD_BASE_ADDRESS + (counter<<1)));
            printf(active_comm_putc, "%c%c", foo&0xff, foo>>8 );
         }
            
      
      #endif

      }

      CMD_STATE = WAITING_FOR_FIRST_HEADER;

   } 
   
   /////////////////////////////////////////////////////////////////////////////
   // if cmd is ready and in an extended command mode
   else if ((CMD_STATE == CMD_READY) && (gblExtCmdMode==1))  {

      printf(active_comm_putc, "%c%c", ReplyHeader1, ReplyHeader2);
      
      // =================================================================
      // cmd = send a Real time clock stream to the computer
      if (gbl_cur_cmd == 0b00000000) {
         for (i=0;i<7;i++) {
            printf(active_comm_putc, "%c", rtcGetItem(i));
         }

      // =================================================================
      // cmd = recieve a real time clock stream from a computer and update
      // the RTC module based on the stream.
      } else if (gbl_cur_cmd == 0b00000001) {
         rtcInit();
         
         for (i=0;i<7;i++) {
            rtcSetItem(i, gblExtCMDBuffer[i]);
         }
         printf(active_comm_putc, "%c", ACK_BYTE);
      }
      
      CMD_STATE = WAITING_FOR_FIRST_HEADER;


   } else   // NO_CMD
   {
      DoSensorStuff();
      DoDisplayModuleStuff();
      DoMotorStuff();
      

     
   }

   
   // if Logo is running
  if (gblLogoIsRunning)
   {
            // if wait command is not in progress -> run next code
            if (!gblWaitCounter) {
               
               evalOpcode(fetchNextOpcode());
            }
   } else if (flgNeedToTurnOffAllMotors) {
         gblMotorONOFF=0;  // turn off all motors by clearing the on,off flag bits
         //ENLow(0); ENLow(1); // turn off all the motor EN pins   
         clearMotors(); // turn off all motors
         flgNeedToTurnOffAllMotors=0;
   }

   ///////////////////////////////////////////////////////////////
   // make sure the CMD_STATE is not stuck somewhere for too long

   // gblCmdTimeOut is incremented in Timer1's ISR every 0.1 sec
   if (gblCmdTimeOut > CMD_TIMEOUT_PERIOD) {
      CMD_STATE = WAITING_FOR_FIRST_HEADER;
      gblCmdTimeOut = 0;
      
   }

   ///////////////////////////////////////////////////////////////
   // Do USB Stuff
   // Most importantly -> check for USB connection with a computer

   usb_task();


   ///////////////////////////////////////////////////////////////
   // Beep when notified by this flag
   // It is currently set by the Logo downloader when Logo download
   // is finished
   if (gblNeedToBeep) {
      beep();
      output_low(USER_LED); // we output high when logo code was downloaded
      gblNeedToBeep = 0;
   }


}

}


#include <logovm.c>
#include <i2cDisplay.c>
#include <bootload.c>
#include <i2c.c>
#include <ds1307.c>
#include <7segment.c>
