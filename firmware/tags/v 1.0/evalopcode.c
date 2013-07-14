//
// evalopcode.c - Contains the Logo Operations
//
// Copyright (C) 2001-2007 Massachusetts Institute of Technology
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


/////////////////////////////////////////////////////
//
//  Logo Virtual Machine
//
//  Written by Arnan (Roger) Sipitakiat
//
//  Logo running on the GoGo Board was created based
//  on the Cricket Logo.
//
//  Cricket Logo creators include:
//    Fred Martin
//    Brian Silverman
//    Mitchel Resnick
//    Robbie Berg
//
/////////////////////////////////////////////////////



void evalOpcode(unsigned char opcode) {

int i;
int16 opr1, opr2, opr3;
unsigned int16 genPurpose=0;


//////////////////////////////////////////////////////////////////////////////
/// if opcode is a prcedure call (msb is 1)

if (opcode & 0b10000000) {


   genPurpose=gblMemPtr+1;  // this is the return address

   // update the mem pointer to point at the procedure
   gblMemPtr = (((unsigned int16)opcode & 0b00111111) << 8) + fetchNextOpcode();

   // then fetch the new opcode

   // The first opcode in a procedure is the number of parameters.
   opr1 = fetchNextOpcode();

   /// if the second msb is set -> tail recursion
   if (opcode & 0b01000000) {
      // - Because this is tail recursion, we have to overwrite preveous procedure inputs
      //   with the new ones. This loop removes the old inputs.
      // - In addition to the procedure input we have three extra entries: the data stack pointer ,
      //   the procedure return address, and the procedure input base address in the input stack.
      for (i=0 ; i < (opr1+3) ; i++) {
         inputPop();
      }
   }

   // Pop the procedure inputs from the main data stack and move them
   // to the input stack
   for (i=0 ; i<opr1 ; i++) {
      inputPush(stkPop());
   }
   inputPush(gblStkPtr);   // save the data stack pointer (we use this with STOP opcode to clear the
                     // data stack of the current procedure.
   inputPush(genPurpose);  // save the return address

   inputPush(gblInputStkPtr - (opr1+2));   // pushes a proc input base address index.
                                  // you'll need to read the documentation
                                  // to fully understand why. Meanwhile, see how it
                                  // is used in case LTHING
                                  // - we add 2 because we also keep the data stack pointer and
                                  //   the return address in this stack.


   //printf("pushed loop adr = %LX, repeat count = %LX\r\n", gblLoopAddress, gblRepeatCount);


   return;
}


////////////////////////////////////////////////////////////////////////////////////////

   switch (opcode) {
      case  CODE_END:
       gblLogoIsRunning = 0;
       output_low(RUN_LED);

       // clear thes variables just in case.
        gblLoopAddress = 0;
       gblRepeatCount = 0;
       break;
      case  NUM8:
       stkPush(fetchNextOpcode());
       break;
      case  NUM16:
       stkPush(((unsigned int16)fetchNextOpcode() << 8) + fetchNextOpcode());
       break;
      case  LIST:
          stkPush(gblMemPtr+1);
          gblMemPtr += fetchNextOpcode();
          //printf("LIST new gblMemPtr = %LX\r\n", gblMemPtr);
       break;
      case  EOL:
       genPurpose = stkPop();
       if (genPurpose > gblMemPtr) {
          gblMemPtr = genPurpose;
        } else {
          gblMemPtr = genPurpose;
          gblRepeatCount = stkPop();   // repeat count
          if (gblRepeatCount > 1) { gblRepeatCount--; }
          if (gblRepeatCount != 1) {
             stkPush(gblRepeatCount);
             stkPush(gblMemPtr);
           }
        }
        delay_ms(10); // add a delay at then end of each loop.
                      // this increases the stability.
        
       //printf("EOL return address = %LX, loop address = %LX\r\n",gblMemPtr, gblLoopAddress);
       break;
      case  EOLR:
       if(stkPop()) {    // if condition is true
         stkPop();        // throw away the loop address
         gblMemPtr = stkPop(); // fetch the next command address
         } else { // if condition if false -> keep on looping.
          gblMemPtr = stkPop();
          stkPush(gblMemPtr);
          delay_ms(10);  // this prevents the waituntil loop to execute too rapidly
                         // which has proven to cause some problems when reading
                         // sensor values.
                 
//          while (1) {output_high(USER_LED); delay_ms(50); output_low(USER_LED); delay_ms(50);}
                       
       }

       break;

     /////////////////////////////////////////////////////////////
     // retrieve procedure input
      case  LTHING:
          genPurpose = fetchNextOpcode();  // index of the input variable
          opr1 = inputPop();  // base address in the input stack
          inputPush(opr1);    // push the base address back to the stack.
          stkPush(gblInputStack[opr1 + genPurpose]);
       break;

     /////////////////////////////////////////////////////////////
     // return to the parent procedure
      case   STOP:
      case OUTPUT:


          if (opcode == OUTPUT) { genPurpose = stkPop(); } // this is the output value

       opr1 = inputPop();  // this is the proc-input stack base address
        gblMemPtr = inputPop(); // this is the return address
       opr2 = inputPop();  // this is the data stack index;




       //printf( "poped loop adr = %LX, repeat count = %LX\r\n", gblLoopAddress, gblRepeatCount);

       // remove any remaining data that belongs to the current procedure from the data stack
       // Usually this is important for the STOP opcode.
       while (gblStkPtr > opr2) { stkPop(); }

       // remove the procedure inputs from the input stack
       while (gblInputStkPtr > opr1) { inputPop(); }

          // Output will push the output to the stack
          if (opcode == OUTPUT) { stkPush(genPurpose); }

       break;



      case  REPEAT:
          gblLoopAddress = stkPop();
          gblRepeatCount = stkPop();

       // these will be poped by EOL
       stkPush(gblMemPtr);  // address after repeat is complete

          if (gblRepeatCount > 1) {
             stkPush(gblRepeatCount);
             stkPush(gblLoopAddress); // address while still repeating
             gblMemPtr = gblLoopAddress;
        } else if (gblRepeatCount == 1) {
           gblMemPtr = gblLoopAddress;
        } else {  // if loop count = 0
             gblMemPtr = stkPop();
        }
       break;
      case  COND_IF:
         opr1=stkPop();  // if true pointer address
         opr2=stkPop();  // condition
        //printf("if %LX goto %LX\r\n", opr2,opr1);
         if (opr2) {
            stkPush(gblMemPtr);
            gblMemPtr=opr1;
         }
         break;
      case  COND_IFELSE:
         opr1=stkPop(); // if false pointer address
         opr2=stkPop(); // if true pointer address
         opr3=stkPop(); // condition
         stkPush(gblMemPtr);
         if (opr3) {
            gblMemPtr=opr2;
         } else {
            gblMemPtr=opr1;
         }

       break;
      case  BEEP:
          beep();
       break;
      case  NOTE:
       break;
      case  WAITUNTIL:
          gblLoopAddress = stkPop();

       // these will be poped by EOLR
       stkPush(gblMemPtr);  // address after repeat is complete
          stkPush(gblLoopAddress); // address while still repeating
          gblMemPtr = gblLoopAddress;

       break;
      case  LOOP:
         gblLoopAddress = stkPop(); // the begining of loop
         gblRepeatCount = 0; // disable this counter (loop forever)
        stkPush(0);   // this distinguishes LOOP from Repeat. (see EOL)
         stkPush(gblLoopAddress); // push loop address back into the stack
                           // so that EOL will loop
           gblMemPtr = gblLoopAddress;
       //printf("LOOP loopaddress = %LX\r\n", gblLoopAddress);
       break;
      case  WAIT:
          gblWaitCounter = stkPop();// the main() loop will pause until
                           // gblWaitCounter is 0. Timer1 ISR
                            // subtracts its value every 0.1 sec.
       break;


      case  TIMER:
         stkPush(gblTimer); // gblTimer increases every 1ms. See in RTCC interrupt
       break;
      case  RESETT:
         gblTimer = 0;
       break;

      // Send will transmit a 16 bit value via the serial port 
      case  SEND:
         genPurpose = stkPop();
         printf(active_comm_putc, "%c%c", genPurpose>>8, genPurpose & 0xff);
       break;
     
     // IR on the gogo USB receives commands from a SONY remote control. It
     // does not use the IR to communicate with a computer like the Cricket.
     case   IR:
       // wait for the new IR code to arrive
       while(!gblNewIRCodeHasArrivedFlag && gblLogoIsRunning) { delay_ms(1); }
       stkPush(gblIRCode);
       gblNewIRCodeHasArrivedFlag = 0;
       break;
     case   NEWIR:
       stkPush(gblNewIRCodeHasArrivedFlag);
       break;

     case   SERIAL:
       // wait for a new Serial byte to arrive
       while(!gblMostRecentlyReceivedByte && gblLogoIsRunning) { delay_us(500); }
       stkPush(gblMostRecentlyReceivedByte);
       gblNewByteHasArrivedFlag = 0;
       break;
     case   NEWSERIAL:
       stkPush(gblNewByteHasArrivedFlag);
       break;


     case  RANDOM:
          stkPush(rand());
       break;

      case  OP_PLUS:
      case  OP_MINUS:
      case  OP_MULTIPLY:
      case  OP_DIVISION:
      case  OP_REMAINDER:
      case  OP_EQUAL:
      case  OP_GREATER:
      case  OP_LESS:
      case  OP_AND:
      case  OP_OR:
      case  OP_XOR:

         opr2=stkPop();  // second operand
         opr1=stkPop();  // first operand
         //printf("op1=%LX\r\n",opr1);
         //printf("op2=%LX\r\n",opr2);
         switch (opcode) {
            case  OP_PLUS:
               opr1+=opr2;
               break;
            case  OP_MINUS:
               opr1-=opr2;
               break;
            case  OP_MULTIPLY:
               opr1*=opr2;
               break;
            case  OP_DIVISION:
               opr1/=opr2;
               break;
            case  OP_REMAINDER:
               opr1%=opr2;
               break;
            case  OP_EQUAL:
               opr1=(opr1==opr2);
               break;
            case  OP_GREATER:
               opr1=(opr1>opr2);
               break;
            case  OP_LESS:
               opr1=(opr1<opr2);
               break;
            case  OP_AND:
               opr1=(opr1&&opr2);
               break;
            case  OP_OR:
               opr1=(opr1||opr2);
               break;
            case  OP_XOR:
               opr1=(opr1^opr2);
               break;
            };
       //printf("result=%LX\r\n", opr1);
       stkPush(opr1);
       break;

      case  OP_NOT:
       stkPush(!stkPop());
       break;


     ///////////////////////////////////////////////////////////////////////
     // Global variables
      case  SETGLOBAL:
          genPurpose = stkPop();  // this is the value
          globalVariables[stkPop()] = genPurpose;
       break;
      case  GETGLOBAL:
         stkPush(globalVariables[stkPop()]);
       break;

      ///////////////////////////////////////////////////////////////////////
      //  Global Array

      case  ASET:
          opr2 = stkPop();      // this is the value to be stored
          opr1 = stkPop() * 2;   // this is the array index. Each entry is two bytes wide.
          genPurpose = ARRAY_BASE_ADDRESS + stkPop();  // this is the base address of the array.

         #if defined(__PCM__)

          write_ext_eeprom(genPurpose + opr1, opr2 & 0xff); // low byte
          write_ext_eeprom(genPurpose + opr1 + 1, opr2 >> 8); // high byte

         #elif defined(__PCH__)
         
          FLASHSetWordAddress( (genPurpose + opr1)<<1);
          FLASHWrite(opr2);
         
         #endif

       break;
      case  AGET:
          opr1 = stkPop() * 2;   // this is the array index. Each entry is two bytes wide.
          genPurpose = ARRAY_BASE_ADDRESS + stkPop();  // this is the base address of the array.

          #if defined(__PCM__)
          
          opr2 = read_ext_eeprom(genPurpose + opr1);
          opr2 |= (int16)read_ext_eeprom(genPurpose + opr1 + 1) << 8;

          #elif defined(__PCH__)
          
          opr2 = read_program_eeprom(genPurpose + opr1); 
          
          #endif

          stkPush(opr2);


       break;

      /////////////////////////////////////////////////////////////////////////
      //  Data collection commands

      case  RECORD:
          genPurpose = stkPop();

          // PCM parts (14 bit PICs like the 16F877) uses an external EEPROM
          // for data Logging storage
          #if defined(__PCM__)
         
          // record the data
          write_ext_eeprom(RECORD_BASE_ADDRESS + gblRecordPtr++, genPurpose & 0xff); // low byte
          write_ext_eeprom(RECORD_BASE_ADDRESS + gblRecordPtr++, genPurpose >> 8); // high byte

          // save current record pointer location to the internal EEPROM
          // we use this for the automatic data upload to the PC
          write_ext_eeprom(MEM_PTR_LOG_BASE_ADDRESS, gblRecordPtr & 0xff);
          write_ext_eeprom(MEM_PTR_LOG_BASE_ADDRESS + 1, gblRecordPtr >> 8);
          
          
          // PCH parts (16 bit PICs like the 18F458) uses the internal flash
          #elif defined(__PCH__)
          
          // record the data
          FLASHSetWordAddress(RECORD_BASE_ADDRESS + ((gblRecordPtr++)<<1));
          FLASHWrite(genPurpose);
          
          // save current record pointer location 
          FLASHSetWordAddress(MEM_PTR_LOG_BASE_ADDRESS);
          FLASHWrite(gblRecordPtr);          
          
          #endif


       break;

      case  RECALL:

          #if defined(__PCM__)

          genPurpose = read_ext_eeprom(RECORD_BASE_ADDRESS + gblRecordPtr++);
          genPurpose |= ((int16)read_ext_eeprom(RECORD_BASE_ADDRESS + gblRecordPtr++) << 8);

         #elif defined (__PCH__)
         
         genPurpose = read_program_eeprom(RECORD_BASE_ADDRESS + (gblRecordPtr<<1) );
         gblRecordPtr++;
                //foo = read_program_eeprom(RECORD_BASE_ADDRESS + (counter<<1));

         #endif
         
          stkPush(genPurpose);
       break;
       
      case  RESETDP:
       gblRecordPtr = 0;
       break;
      
      case  SETDP:
       gblRecordPtr = stkPop();
       break;
      
      case  ERASE:
          opr1 = stkPop() * 2;
          for (genPurpose=0 ; genPurpose<opr1 ; genPurpose++) {

             #if defined(__PCM__)
             write_ext_eeprom(RECORD_BASE_ADDRESS + genPurpose, 0);

             #elif defined(__PCH__)
             FLASHSetWordAddress(RECORD_BASE_ADDRESS + genPurpose);
             FLASHWrite(0);             

             #endif


        }
        gblRecordPtr = 0;
       break;




     /////////////////////////////////////////////////////////////////
     //  Due to the PIC's memory architecture, we need to split this
     //  procedure so that it isn't too big to fit one memory segment.
      default:  evalOpcode2(opcode);
      };
}



#separate void evalOpcode2(unsigned char opcode) {

int i=0;
int16 opr1, opr2, opr3;
unsigned int16 genPurpose=0;
char *strPtr;  // used with the show long string command
int strLen;    //  used with the show long string command
char displayModuleBuffer[33]; // 16x2 LCD display buffer. The 33 location is for the null terminating char

   switch (opcode) {







      case  WHEN:
       break;
      case  WHENOFF:
       break;
      case  M_A:
          gblActiveMotors = 0b00000001;  // set bit 0
       break;
      case  M_B:
          gblActiveMotors = 0b00000010;  // set bit 1
       break;
      case  M_AB:
          gblActiveMotors = 0b00000011;
       break;

      //////////////////////////////////////////////////////

     // Look at how M_ON, M_ONFOR, and M_OFF work carefully.
     // - M_ON, M_ONFOR starts by turning motors on.
     // - M_ON breaks right after while M_ONFOR continues.



      case  M_OFF:         i++;
      case  M_THATWAY:     i++;
      case  M_THISWAY:     i++;
      case  M_RD:          i++;
      case  BRAKE:         i++;
      case  M_ON:
      case  M_ONFOR:

// Moved to motorCortol() instead
//         SetMotorMode(MOTOR_NORMAL);
         MotorControl(i);

         if (opcode == M_ONFOR) {
           gblWaitCounter = stkPop(); // the main() loop will pause until
                               // gblWaitCounter is 0. Timer1 ISR
                               // subtracts its value every 0.1 sec.

            gblONFORNeedsToFinish = 1; // this will cause fetchNextOpcode()
         }

 //        printf( "i=%d\r\n",i);

         break;


     ////////////////////////////////////////////////////

      case  SETPOWER:
       SetMotorPower(stkPop());
       break;

      case  BSEND:
      case  BSR:
         // These two opcodes are not supported.
         // So, halt the board if they are used.
      Halt();
      break;
      case  M_C:
          gblActiveMotors = 0b00000100;  // set bit 2
       break;
      case  M_D:
          gblActiveMotors = 0b00001000;  // set bit 3
       break;
      case  M_CD:
          gblActiveMotors = 0b00001100;
       break;
      case  M_ABCD:
       gblActiveMotors = 0b00001111;
       break;
//      case  FASTSEND:
//       break;

      case  REALLY_STOP:
       gblLogoIsRunning = 0;
       output_low(RUN_LED);
       break;

      case  EB:  // reads byte from memory
           #if defined(__PCM__)
              stkPush(read_ext_eeprom(stkPop()));
           #elif defined(__PCH__)
              stkPush(read_program_eeprom(stkPop()));
           #endif
       break;

      case  DB:  // deposit byte to memory
           /// Note: I have checked this code. I might have swapped opr1 and opr2
           opr1 = stkPop(); // value to write
           opr2 = stkPop(); // memory address
           
          #if defined(__PCM__)
             write_ext_eeprom(opr2,opr1);

          #elif defined(__PCH__)
             FLASHSetWordAddress(opr2);
             FLASHWrite(opr1);             

          #endif           
        
          // write_ext_eeprom(opr2,opr1);
       break;

      case  LOW_BYTE:  // returns low byte
           stkPush(stkPop() & 0xff);
       break;
      case  HIGH_BYTE:  // returns high byte
           stkPush(stkPop() >> 8);
       break;


   ///////////////////////////////////////////////////
   //  The following code are unique to the GoGo board

      /// read sensor
      case  SENSOR1:
      case  SENSOR2:
      case  SENSOR3:
      case  SENSOR4:
      case  SENSOR5:
      case  SENSOR6:
      case  SENSOR7:
      case  SENSOR8:

       // we need the following IF because the opcode for sensor1&2 are separate from the rest.
       // If this wasn't the case we could have just done .. i = opcode - SENSOR1;
       if (opcode < SENSOR3) { i = opcode - SENSOR1; }
       else { i = opcode - SENSOR3 + 2; }

         stkPush(readSensor( gblSensorPortMap[i] ));  // map sensor number to the actual analog port on the processor
       break;

      // read sensor and treat it as a on-off switch (0 or 1)
      case  SWITCH1:
      case  SWITCH2:
      case  SWITCH3:
      case  SWITCH4:
      case  SWITCH5:
      case  SWITCH6:
      case  SWITCH7:
      case  SWITCH8:

       if (opcode < SWITCH3) { i = opcode - SWITCH1; }
       else { i = opcode - SWITCH3 + 2; }

         stkPush(!(readSensor(gblSensorPortMap[i])>>9));
       break;


     /////////////////////////////////////////////////////////////
     //  user LED control
     case ULED_ON:
         USER_LED_ON;
         break;
     case ULED_OFF:
         USER_LED_OFF;
         break;


     /////////////////////////////////////////////////////////////
     //  Servo controls

     case SERVO_SET_H:
     case SERVO_LT:
     case SERVO_RT:


         // Caution: SetMotorMode() must be called AFTER the
         // MotorControl() commands

         MotorControl(MTR_ON);
         MotorControl(MTR_THISWAY);
         SetMotorMode(MOTOR_SERVO);

         i = stkPop();

         if (opcode == SERVO_SET_H) {
            SetMotorPower(i);
         } else if (opcode == SERVO_LT)
            ChangeMotorPower(i);
         else
            ChangeMotorPower(-1*i);

         break;

      case TALK_TO_MOTOR:
         opr1 = stkPop(); // this is the motor bits
         TalkToMotor (opr1);
         break;


      ///////////////////////////////////////////////////////////
      //
      //  I2C  Commands
      //

      case CL_I2C_START:
         gblI2CisBusy = 1;  // reserve the i2c bus
         i2c_start();
         break;

      case CL_I2C_STOP:
         i2c_stop();
         gblI2CisBusy = 0;  // releases the i2c bus
         break;

      case CL_I2C_WRITE:
         i2c_write(stkPop());
         break;

      case CL_I2C_READ:
         stkPush(i2c_read(stkPop()));
         break;

      ///////////////////////////////////////////////////////////
      // Initialize the RTC clock as follows:
      // - enable the clock. This needs to be cone every time the 
      //   RTC module looses power
      // - set time to 24 hrs mode (as opposed to the AM/PM mode)
      // - enable the square wave output on the RTC chip at 1Hz
      
      case RTC_INIT:
         rtcInit();         
         break;
      
      // Get one byte from the RTC clock
      
      case RTC_GET_ITEM:
         stkPush(rtcGetItem(stkPop()));
     
         break;
         
     ////////////////////////////////////////////////////////////
     // Display Module show command
     // Works with both the 7-Segment and 16x2 LCD modules
     
      case DISPLAY_SHOW:
         opr1 = stkPop(); // this tells us what to do
         if (opr1 == DISPLAY_CMD_SEND_VALUE) { // send a 16-bit value
            displayValue(stkPop());
            delay_us(300);   // must delay to allow the display to process the input value
                             // before sending any further i2c commands. This display module
                             // has to convert this number into a string and copy it to the
                             // display buffer. This is a time consuming process. Without this
                             // delay, it is possible to cause an overrun error.
                             
         } else if (opr1 == DISPLAY_CMD_SEND_TEXT) { // send a 4-letter text (7-segment)
            displayText(stkPop(),stkPop(),stkPop(),stkPop());         
         } else if (opr1 == DISPLAY_CMD_SEND_LONG_TEXT) { // send a long text (LCD display)
            strLen = stkPop();   // get the text length
            
            for (i=0;i<strLen;i++) {
               displayModuleBuffer[i] = stkPop();
            }
            displayModuleBuffer[i] = 0;  // null terminating character
            displayLongText(displayModuleBuffer); // send the string to the display
         }
         
         break;
 
      case DISPLAY_CLS:
         clearDisplay();
         break;
 
      case DISPLAY_GETPOS:
         stkPush(getDisplayPos());
         break;

      case DISPLAY_SETPOS:
         setDisplayPos(stkPop());
         break;

      case TALK_TO_7SEG_1:
         gblAutoDetectDisplays=0;
         gblDisplayAddress=DISPLAY_7SEG_ADDR_1;
         break;

      case TALK_TO_7SEG_2:
         gblAutoDetectDisplays=0;
         gblDisplayAddress=DISPLAY_7SEG_ADDR_2;
      
         break;

      case TALK_TO_LCD_1:
         gblAutoDetectDisplays=0;
         gblDisplayAddress=DISPLAY_LCD_ADDR_1;
      
         break;

      case TALK_TO_LCD_2:
         gblAutoDetectDisplays=0;
         gblDisplayAddress=DISPLAY_LCD_ADDR_2;
      
         break;

 
 
     default:
      Halt();


};

}
