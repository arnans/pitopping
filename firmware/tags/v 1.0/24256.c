///////////////////////////////////////////////////////////////////////////
////   Library for a 24LC256 serial EEPROM                             ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);   Read the byte d from the address a    ////
////                                                                   ////
////   The main program may define eeprom_sda                          ////
////   and eeprom_scl to override the defaults below.                  ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////

#ifndef EEPROM_SDA

#endif



#define EEPROM_ADDRESS 0xa0
#define EEPROM_SIZE   32768
//#define byte unsigned char

void init_ext_eeprom()
{
   output_float(I2C_SCL);
   output_float(I2C_SDA);

}


void write_ext_eeprom(long int address, byte data)
{
   short int status;
   i2c_start();
   i2c_write(EEPROM_ADDRESS);
   i2c_write(address>>8);
   i2c_write(address);
   i2c_write(data);
   i2c_stop();

   // min delay time tested to work was 2 ms.
   // using 5 ms just for safety.
   
   delay_ms(5);


   // The above delay was added because CCS's pooling code
   // below doesn't work when complied with their
   // newer compiler (4.0). It worked with older versions (3.x) 

//   i2c_start();
//   status=i2c_write(EEPROM_ADDRESS);
//   while( (status==1))
//   {
//   i2c_start();
//   status=i2c_write(EEPROM_ADDRESS);
//   }

}


byte read_ext_eeprom(long int address) {
   byte data;
   i2c_start();
   i2c_write(EEPROM_ADDRESS);
   i2c_write(address>>8);
   i2c_write(address);
   i2c_start();
   i2c_write(0xa1);
   data=i2c_read(0);
   i2c_stop();
   return(data);
}

