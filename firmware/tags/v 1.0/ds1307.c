#define DS1307_ADDRESS  0xD0    // DS1307 I2C Address (Not changable)


BYTE bin2bcd(BYTE binary_value) 
{ 
  BYTE temp; 
  BYTE retval; 

  temp = binary_value; 
  retval = 0; 

  while(1) 
  { 
    // Get the tens digit by doing multiple subtraction 
    // of 10 from the binary value. 
    if(temp >= 10) 
    { 
      temp -= 10; 
      retval += 0x10; 
    } 
    else // Get the ones digit by adding the remainder. 
    { 
      retval += temp; 
      break; 
    } 
  } 

  return(retval); 
} 


// Input range - 00 to 99. 
BYTE bcd2bin(BYTE bcd_value) 
{ 
  BYTE temp; 

  temp = bcd_value; 
  // Shifting upper digit right by 1 is same as multiplying by 8. 
  temp >>= 1; 
  // Isolate the bits for the upper digit. 
  temp &= 0x78; 

  // Now return: (Tens * 8) + (Tens * 2) + Ones 

  return(temp + (temp >> 2) + (bcd_value & 0x0f)); 
} 


// Initialize the RTC
//
// The CH bit is bit 7 at register address 0.
// The bit is set by default when the DS1307 looses power
// We need to clear this bit to enable the chip (making the clock tick).
//
// We also disabl the squarewave output by clearing bit 4 of register 7

void rtcInit() {

   int temp;

   // Clear the CH bit
   // ==============================
   // read the current seconds value
   i2cRead(DS1307_ADDRESS, 0, *temp);
   // clear bit 7
   temp &= 0x7F;
   delay_us(5); 
   // write back the secnds value
   i2cWrite(DS1307_ADDRESS,0,temp);


   // Set hours to a 24 hr format
   // =============================
   // read the current hours value
   i2cRead(DS1307_ADDRESS, 2, *temp);
   // clear bit 6 to set 24hr mode
   bit_clear(temp,6);
   delay_us(5);
   i2cWrite(DS1307_ADDRESS,2,temp);


   //set the squarewave output freq to 1 Hz
   // - set SQWE bit to 1
   // - set RS1, RS0 to 0 0 for 1 Hz output
   i2cWrite(DS1307_ADDRESS,7,0x10);
  

}

// Read one byte from the RTC register. Here's the layout
//
// Address     Description
// ====================================
// 0           seconds
// 1           minutes
// 2           hours (24 hr format)
// 3           Day of Week (DOW)
// 4           Date
// 5           Month
// 6           Year (0 = the year 2000)

int rtcGetItem(int itemNumber) {

   int temp;

   if (i2cRead(DS1307_ADDRESS,itemNumber,&temp) != SUCCESS) {
      return(100);   // 100 is the error code since nothing can be that high
   }
   return(bcd2bin(temp));

}

void rtcSetItem(int itemNumber, itemValue) {
   i2cWrite(DS1307_ADDRESS, itemNumber, bin2bcd(itemValue));
}


/*
void rtcGetSeconds(int *sec) {
   i2cRead(DS1307_ADDRESS,0,sec);
   *sec = bcd2bin(*sec);
}

void rtcGetMinutes(int *min) {
   i2cRead(DS1307_ADDRESS, 1, min);
   *min = bcd2bin(*min);
}

void rtcGetHours(int *hr) {
   i2cRead(DS1307_ADDRESS, 2, hr);
   *hr = bcd2bin(*hr & 0x3F));
}

void rtcGetDow(int *dow) {
   i2cRead(DS1307_ADDRESS, 3, dow);
}

void rtcGetDay(int *day) {
   i2cRead(DS1307_ADDRESS, 4, day);
   *day = bcd2bin(*day);
}

void rtcGetMonth(int *mo) {
   i2cRead(DS1307_ADDRESS, 5, mo);
   *mo = bcd2bin(*mo);
}

void rtcGetYear(int *year) {
   i2cRead(DS1307_ADDRESS, 6, year);
   *year = bcd2bin(*year);
}



void ds1307_set_date_time(BYTE day, BYTE mth, BYTE year, BYTE dow, BYTE hr, BYTE min, BYTE sec) 
{ 
  sec &= 0x7F; 
  hr &= 0x3F; 

  i2c_start(); 
  i2c_write(0xD0);            // I2C write address 
  i2c_write(0x00);            // Start at REG 0 - Seconds 
  i2c_write(bin2bcd(sec));      // REG 0 
  i2c_write(bin2bcd(min));      // REG 1 
  i2c_write(bin2bcd(hr));      // REG 2 
  i2c_write(bin2bcd(dow));      // REG 3 
  i2c_write(bin2bcd(day));      // REG 4 
  i2c_write(bin2bcd(mth));      // REG 5 
  i2c_write(bin2bcd(year));      // REG 6 
  i2c_write(0x80);            // REG 7 - Disable squarewave output pin 
  i2c_stop(); 
} 

void ds1307_get_date(BYTE *day, BYTE *mth, BYTE *year, BYTE *dow) 
{ 
  i2c_start(); 
  i2c_write(0xD0); 
  i2c_write(0x03);            // Start at REG 3 - Day of week 
  i2c_start(); 
  i2c_write(0xD1); 
  *dow  = bcd2bin(i2c_read() & 0x7f);   // REG 3 
  *day  = bcd2bin(i2c_read() & 0x3f);   // REG 4 
  *mth  = bcd2bin(i2c_read() & 0x1f);   // REG 5 
  *year = bcd2bin(i2c_read(0));            // REG 6 
  i2c_stop(); 
} 

void ds1307_get_time(BYTE *hr, BYTE *min, BYTE *sec) 
{ 
  i2c_start(); 
  i2c_write(0xD0); 
  i2c_write(0x00);            // Start at REG 0 - Seconds 
  i2c_start(); 
  i2c_write(0xD1); 
  *sec = bcd2bin(i2c_read() & 0x7f); 
  *min = bcd2bin(i2c_read() & 0x7f); 
  *hr  = bcd2bin(i2c_read(0) & 0x3f); 
  i2c_stop(); 

} 

/////////////////////////////////////////////////////////////////////
//  Read a register value from the DS1307
//
//  Note: It is important that we use a second i2c_start() when
//        we want to switch the data flow from master->slave to 
//        master<-slave. This i2c_start() will actually generate
//        a "re-start" condition on the bus. DO NOT use i2c_stop()
//        before this command. The program will not work.
/////////////////////////////////////////////////////////////////////

int readRTC(int registerAddress, int *registerValue) {

   disable_interrupts(GLOBAL);

   i2c_start();

   // if i2c_write does not return 0 -> it means we did not get an ACK back
   // from the slave. It means the slave is not present or is not working.


   
   if (i2c_write(TARGET_ADDRESS | 0) !=0) {
      resetI2C();  // reset the bus (this is optional)
      enable_interrupts(GLOBAL);
      return(NOT_SUCCESS);
      
   }

  
   // set the register pointer on the DS1307
   i2c_write(registerAddress);
   
   i2c_start();
   
   // tell the slave we want to read from it by setting bit 0 of the slave
   // address to 1.
   i2c_write(TARGET_ADDRESS | 1);
   
   *registerValue = i2c_read(0); // read from the DS1307. Parameter 0 tells i2c_read() 
                           // to do a "not acknowledge" or NACK read which tells the 
                           // DS1307 to end a read transaction. 
                           // Note. Without the NACK, you can continually read
                           // from the DS1307, getting the sequence of register
                           // values.
   i2c_stop();

   enable_interrupts(GLOBAL);

   return(SUCCESS);
   
}

*/
