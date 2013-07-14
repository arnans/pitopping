void resetI2C();
int i2cWrite(int slaveAddress, int registerAddress, int registerValue);
int i2cRead(int slaveAddress, int registerAddress, int *registerValue);
