//
// evalopcode.c - Display module plug-in routines
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



int displayPing() {
   int inbyte=0;
   int found=0;
   int i;
   int currentAddress;
   int displayAddress=0;

   
   // probe for any connected displays
   for (i=0;i<4;i++) {
      // fetch a display address to probe
      currentAddress = gblDisplayAddressList[i];
      ///disable_interrupts(GLOBAL);
      i2c_start();
      // We write to the Display address and check the return value
      // if i2c_write returns 0 -> it means we got an ACK back
      // so it means the display module is present.
      found = i2c_write(currentAddress);
      found = !found;
  
      if (found) {
         displayAddress = currentAddress;
         i2c_write(DISPLAY_CMD_PING);  // tell the slave we are just pinging.
         i2c_stop();
         ///enable_interrupts(GLOBAL);
         break;
      }
      i2c_stop();
      ///enable_interrupts(GLOBAL);
      
      delay_us(10); // delay to give time to other interrupts to
                    // do their work.
   }


   return(displayAddress);
   
}

void displayValue(int16 val) {

   int temp;

   // show on external 7-segment 
   if (gblUseOnBoard7Segment == 0) {
   
      i2c_start();
   
      // We write to the Display address and check the return value
      // if i2c_write returns 0 -> it means we got an ACK back
      // so it means the display module is present.
      if (i2c_write(gblDisplayAddress)) {
         ///enable_interrupts(GLOBAL);
         return;
      } 
      
      i2c_write(DISPLAY_CMD_SEND_VALUE);
      i2c_write((int)(val>>8));  // high byte
      i2c_write((int)(val & 0xFF));  // low byte
      i2c_stop();
   } 
   
   // show on built-in 7-segment display
   else if (gblUseOnBoard7Segment == 1) {

        cls_internal7Seg(); // make the screen blank

         // first digit 
         temp = val - (val/10)*10;
         gblCharCodeToShow1 = Char2Segments(temp);

         if (val<10) {return;}
         // second digit
         val /= 10;
         temp = val - (val/10)*10;
         gblCharCodeToShow2 = Char2Segments(temp);
      
         if (val<10) {return;}
         // third digit
         val /= 10;
         temp = val - (val/10)*10;
         gblCharCodeToShow3 = Char2Segments(temp);

         if (val<10) {return;}
         // fourth digit
         val /= 10;
         temp = val - (val/10)*10;
         gblCharCodeToShow4 = Char2Segments(temp);
   
   }
}


void displayText(char ch1, char ch2, char ch3, char ch4) {

   // show on external 7-segment 
   if (gblUseOnBoard7Segment == 0) {
   
      i2c_start();
   
      // We write to the Display address and check the return value
      // if i2c_write returns 0 -> it means we got an ACK back
      // so it means the display module is present.
      if (i2c_write(gblDisplayAddress)) {
         ///enable_interrupts(GLOBAL);
         return;
      } 
      
      i2c_write(DISPLAY_CMD_SEND_TEXT);
      i2c_write(ch4);  
      i2c_write(ch3);  
      i2c_write(ch2);  
      i2c_write(ch1);  
      i2c_stop();
   
   }
   // show on built-in 7-segment display
   else if (gblUseOnBoard7Segment == 1) {
      showOnBuiltIn7Segment(ch1,ch2,ch3,ch4);
   }
}

void displayLongText(char *text) {

   signed int i=0;

   ///disable_interrupts(GLOBAL); 

   i2c_start();

   // We write to the Display address and check the return value
   // if i2c_write returns 0 -> it means we got an ACK back
   // so it means the display module is present.
   if (i2c_write(gblDisplayAddress)) {
      ///enable_interrupts(GLOBAL);
      return;
   } 
   
   i2c_write(DISPLAY_CMD_SEND_LONG_TEXT);
   
   // write the display string (in reverse order)
   for(i=0;text[i] != '\0';i++)
   {
      i2c_write(text[i]);     
   }
   i2c_write('\0');
   i2c_stop();
   ///enable_interrupts(GLOBAL);
}




int gblDisplaySensorCounter=0;  // tracks which sensor we have to send next

void displaySendSensors(void) {

   int16 curVal;
   int temp;


   ///disable_interrupts(GLOBAL); 

   curVal = readSensor(gblSensorPortMap[gblDisplaySensorCounter]);
   
   // send sensor values only if it is less than 1020. This is 
   // to minimize the i2c traffic. We assume a value >= 1020 
   // means there is no sensor attached to the port.
   // But always send at least sensor1's value.
   if ( (curVal < 1020) || (gblDisplaySensorCounter == 0)) {

      // send command
      i2c_start(); 
      i2c_write(gblDisplayAddress); 
      i2c_write(DISPLAY_CMD_UPDATE_SENSORS);    

      // send high byte 
      temp =  (int)(curVal >> 8) | (gblDisplaySensorCounter<<5);
      i2c_write (temp);
      // low byte
      i2c_write( (int)(curVal & 0xff) );
      i2c_stop();       
//      delay_us(400);  // must wait here
      
   }
   gblDisplaySensorCounter++;
   gblDisplaySensorCounter %= 8;

   ///enable_interrupts(GLOBAL); 
   
}


// ===================================================================
// LCD Specific commands

void clearDisplay(){
   ///disable_interrupts(GLOBAL);
   i2c_start();
   i2c_write(gblDisplayAddress);
   i2c_write(DISPLAY_CMD_CLS);
   i2c_stop();
   ///enable_interrupts(GLOBAL);
}

int getDisplayPos(){
   int pos=0;
   ///disable_interrupts(GLOBAL);
   i2c_start();
   i2c_write(gblDisplayAddress|0x01);   
   pos=i2c_read(0);
   i2c_stop();
   ///enable_interrupts(GLOBAL);
 
   return pos;
}


void setDisplayPos(int pos){
   ///disable_interrupts(GLOBAL);
   i2c_start();
   i2c_write(gblDisplayAddress);
   i2c_write(DISPLAY_CMD_SETPOS);
   i2c_write(pos);  
   i2c_stop();
   ///enable_interrupts(GLOBAL);
}



