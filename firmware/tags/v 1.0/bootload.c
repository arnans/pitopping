/////////////////////////////////////////////////////////////////////////////
//
//       =================================================
//
//       This code is released under the GPL license. Read 
//       the attached license for full information but here's 
//       the basic idea:
//        - Use it freely
//        - Let others use your new/modified code freely too
//        - So, please release the source
//
//       =================================================
//
//
/////////////////////////////////////////////////////////////////////////////

//#define writeBufferSize getenv("FLASH_ERASE_SIZE") >> 1  // = 16 on PCM (16F887)


// a slim version of atoi().
// converts ascii text to integer
// i.e. '1' = 1, 'A' = 10
unsigned int a2i(unsigned char asciiByte) {

      if (asciiByte >= 'A' && asciiByte <= 'F')
         return((asciiByte) - 'A' + 10);
      else if (asciiByte >= '0' && asciiByte <= '9')
         return( asciiByte - '0');
}


// convert two ascii text to a 8 bit integer
unsigned int read8() {
     return( (a2i(usb_cdc_getc()) << 4) + (a2i(usb_cdc_getc())) );
}


// Writes data to flash memory using block mode
// On PIC16F887, Write Size = 8 Words (16 Bytes), Erase size = 16 Words (32 Bytes)
// On PIC18F458, Write Size = 8 Bytes, Erase size = 64 Bytes
// - Whenever we write to the beginning of an erase block, the entire block (16 words) will be
//   automatically erased (set to 0x3FFF). 
// - So, whenever we want to write some bytes to the flash memory, we need to read the entire
//   erase block into a buffer, modify that buffer, then write it back to the flash memory.



void writeFlash(int16 memoryBlockAddress, int16 positionInMemoryBlock, int16 len, int *Buffer) {

   int writeBuffer[getenv("FLASH_ERASE_SIZE")];
   int16 i, counter;
   int16 writeLenInThisBlock;
   int1 notDone = 1;
   
   
   
  
   
   do {
   
      // read the entire erase block to memory
      read_program_memory(memoryBlockAddress, writeBuffer, getenv("FLASH_ERASE_SIZE"));
      
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

      // write the block back to the flash memory. 
      for (i=0 ; i< (getenv("FLASH_ERASE_SIZE") / getenv("FLASH_WRITE_SIZE")); i++) {
         disable_interrupts(GLOBAL);
         write_program_memory( memoryBlockAddress + (i * getenv("FLASH_WRITE_SIZE")) ,
                               writeBuffer + (i * getenv("FLASH_WRITE_SIZE")), 
                               getenv("FLASH_WRITE_SIZE"));
         enable_interrupts(GLOBAL);
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
   } while (notDone);


}

int waitFor(char targetByte) {
   int tollerance=100;  // how many bytes to ignore before giving up
   
   while (tollerance > 0) {
      while (!usb_cdc_kbhit()) {
          delay_ms(1);

//         usb_task();   // not really neccessary if usb_init() is used instead of usb_init_cs()
//         delay_ms(1);
      }
      
      if (usb_cdc_getc() == targetByte) {
         return(1);
      }
      
      tollerance--;
   }
   return(0);

}


////////////////////////////////////////////////////////////////////////////////
//
//  Download a bootloader firmware to the PIC. It will overwrite parts of the 
//  gogo firmware used to store data. The bootloader should be downloaded only
//  when we want to upgrade the gogo's firmware. Once downloaded, it can then
//  receive and program the new gogo firmware to the flash memory.
//
////////////////////////////////////////////////////////////////////////////////


void downloadBootloader()
{
   int Buffer[SerBufferSize];    // serial input buffer   

   int1 notDone = 1;

   unsigned int recLen;   // HEX file record length
   unsigned int16 writeAddr;  // HEX file write address
   unsigned char recType;  // HEX file record type

   unsigned char i=0,j;   // general counters
///   unsigned int16 UserBootVectorAddr; // holds the address of our new boot vector
   unsigned int positionInMemoryBlock;
   unsigned int16 memoryBlockAddress;
   unsigned int bufferIndex;
   unsigned int writeLen;
   
   int16 BankAddress;
   int1 skipWriting=0;
   
   int relocateCount=0;
   #define REC_SIZE  0x10
   int relocateRecordLen[6];
   int relocateOriginalAddress[6];

//   int flashReadBuffer[getenv("FLASH_ERASE_SIZE")];  // buffer for the relocated first mem block
   int flashReadBuffer[REC_SIZE];  // buffer for the relocated first mem block

      usb_init();
      while(!usb_cdc_connected());

      while (notDone) {


         //////////////////////////////////////////
         /// Wait for ':'

         do {
            while (!usb_cdc_kbhit());
         } while (usb_cdc_getc() != ':');



         /////////////////////////////////////////
         //  Record length

         recLen = read8();

         /////////////////////////////////////////
         //  Write Address

         writeAddr  = ((int16)read8() << 8) + read8();

         /////////////////////////////////////////
         //  Rec Type

         usb_cdc_getc();  // ignore the first digit, it is always '0'
         recType = usb_cdc_getc();

         if (recType == '1') { // End of file record
            notDone = 0;

         } else if (recType == '4') {  // bank select record

            BankAddress = (read8()<<8) + read8();
            if (BankAddress != 0)
               skipWriting = 1;
            else
               skipWriting = 0;
            
         } else if ((recType == '0') && !(skipWriting)) { // data record
            

            /// get the data
            for (i=0; i < recLen ; i++) {
                Buffer[i] = read8();
                
            }
 
/*            // if data is in the EEPROM area -> do nothing
            if ((writeAddr >= 0x2100) && (writeAddr <= 0x21FF)) {
               // we don't support EEPROM records yet
            }
*/


            // if writing to the first memory block -> we need to temporarily 
            // relocate it elsewhere. Since this area contains the boot and 
            // interrupt vectors -> it should be written as the very last memory 
            // block.
            
            if (writeAddr & (0xFFFF ^ (getenv("FLASH_ERASE_SIZE")-1)) == 0) {
               relocateOriginalAddress[relocateCount] = writeAddr; // remmber the orginal location
               relocateRecordLen[relocateCount] = recLen; // remember each record's len

               writeAddr = RESERVED_MEMORY_START + (relocateCount * REC_SIZE);
               relocateCount++;
            } 
            
            positionInMemoryBlock = writeAddr & (getenv("FLASH_ERASE_SIZE")-1);
            memoryBlockAddress = writeAddr & (0xFFFF ^ (getenv("FLASH_ERASE_SIZE")-1));
            writeFlash(memoryBlockAddress, positionInMemoryBlock, recLen, Buffer);

         }
         
         if (notDone)
            // Tells the PC to send the next line
            printf(active_comm_putc, "%c",READY_FOR_NEXT);

      }
 
       // Tells the PC that we are finished
      printf(active_comm_putc,"%c", FINISH_FLAG);
 
      delay_ms(100);
      
      /////////////////////////////////////////////////////////////////////////
      //
      //  Now we move the relocated first block to its original place 
      //

      // read the entire flash block to memory

      disable_interrupts(GLOBAL);

      for (i=0;i<relocateCount;i++) {

         read_program_memory(RESERVED_MEMORY_START + (i*REC_SIZE), flashReadBuffer, REC_SIZE);

         writeAddr = relocateOriginalAddress[i];
         writeFlash(0x0, writeAddr, relocateRecordLen[i], flashReadBuffer );
      }

      output_low(USER_LED);
      delay_ms(100);

      reset_cpu();


}









