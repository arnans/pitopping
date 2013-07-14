 // define possible reply bytes
#define READY_FOR_NEXT  0x11
#define FINISH_FLAG     0x55
#define BOOTLOADER_OVERWRITE  0x80
#define ERROR_TIMEOUT   0x88


#define SerBufferSize 16  // serial input buffer size

unsigned int a2i(unsigned char asciiByte);
unsigned int read8();
void writeFlash(int16 memoryBlockAddress, int16 positionInMemoryBlock, int16 len, int *Buffer);
int waitFor(char targetByte);
void downloadBootloader();
