
// Registers used to check and clear I2C errors
#bit SSPEN = 0x14.5
#bit SSPOV = 0x14.6
#bit WCOL  = 0x14.7

#define SUCCESS      1
#define NOT_SUCCESS  0


void resetI2C() {

   // clear the error flag registers and re-enable the i2c bus
   SSPEN = 0;   // disable i2c
   SSPOV = 0;   // clear the receive overflow flag
   WCOL = 0;    // clear the write collision flag

   SSPEN = 1;   // re-enable i2c   

}


/*
/////////////////////////////////////////////////////////////////////
//  Set register pointer will send one byte to the slave
/////////////////////////////////////////////////////////////////////

int i2cSetRegisterPointer(int slaveID, int registerAddress) {

   disable_interrupts(GLOBAL);

   i2c_start();

   // if i2c_write does not return 0 -> it means we did not get an ACK back
   // from the slave. It means the slave is not present or is not working.

   if ( i2c_write(slaveID | 0) != 0) {
      resetI2C();  // reset the bus (this is optional)
      enable_interrupts(GLOBAL);

      return(NOT_SUCCESS);
   }

   // set the register pointer 
   i2c_write(registerAddress);

   i2c_stop();
   enable_interrupts(GLOBAL);

   return(SUCCESS);
   
}
*/

///////////////////////////////////////////////////////////////////////////
//
// Write one byte to the given register address
//
///////////////////////////////////////////////////////////////////////////


int i2cWrite(int slaveAddress, int registerAddress, int registerValue) {
  ///disable_interrupts(GLOBAL);

   i2c_start();

   // if i2c_write does not return 0 -> it means we did not get an ACK back
   // from the slave. It means the slave is not present or is not working.


   
   if (i2c_write(slaveAddress | 0) !=0) {
      resetI2C();  // reset the bus (this is optional)
      ///enable_interrupts(GLOBAL);
      return(NOT_SUCCESS);
      
   }

  
   // set the register pointer 
   i2c_write(registerAddress);
   // write a value to that location
   i2c_write(registerValue);
   i2c_stop();
   ///enable_interrupts(GLOBAL); 

   return(SUCCESS);
   


}


/////////////////////////////////////////////////////////////////////
//  Read a register value at a given address from slave
//
//  Note: It is important that we use a second i2c_start() when
//        we want to switch the data flow from master->slave to 
//        master<-slave. This i2c_start() will actually generate
//        a "re-start" condition on the bus. DO NOT use i2c_stop()
//        before this command. The program will not work.
/////////////////////////////////////////////////////////////////////

int i2cRead(int slaveAddress, int registerAddress, int *registerValue) {

   ///disable_interrupts(GLOBAL);

   i2c_start();

   // if i2c_write does not return 0 -> it means we did not get an ACK back
   // from the slave. It means the slave is not present or is not working.


   
   if (i2c_write(slaveAddress | 0) !=0) {
      resetI2C();  // reset the bus (this is optional)
      ///enable_interrupts(GLOBAL);
      return(NOT_SUCCESS);
      
   }

  
   // set the register pointer on the slave device
   i2c_write(registerAddress);
   
   i2c_start();
   
   // tell the slave we want to read from it by setting bit 0 of the slave
   // address to 1.
   i2c_write(slaveAddress | 1);
   
   *registerValue = i2c_read(0); // read from the slave. Parameter 0 tells i2c_read() 
                           // to do a "not acknowledge" or NACK read which tells the 
                           // slave to end a read transaction. 
                           // Note. Without the NACK, you can continually read
                           // from the slave, getting the sequence of register
                           // values.
   i2c_stop();

   ///enable_interrupts(GLOBAL);

   return(SUCCESS);
   
}


