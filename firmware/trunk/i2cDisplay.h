//#define DISPLAY_ADDRESS 0xB0  // display address is dynamic depending on 
                                // which module is being connected
#define DISPLAY_CMD_PING  1
#define DISPLAY_CMD_SEND_VALUE 2
#define DISPLAY_CMD_SEND_TEXT   3
#define DISPLAY_CMD_UPDATE_SENSORS 4
#define DISPLAY_CMD_SEND_LONG_TEXT  5
#define DISPLAY_CMD_CLS 6
#define DISPLAY_CMD_GETPOS 7
#define DISPLAY_CMD_SETPOS 8

int displayPing();
void displayValue(int16 val);
void displayText(char ch1, char ch2, char ch3, char ch4);
void displayLongText(char *text);
void displaySendSensors(void);
void clearDisplay();
int getDisplayPos();
void setDisplayPos(int pos);

// 1 -> use the pi-topping builtin 7segment display
// 0 -> use external 7segment via i2c 
int1 gblUseOnBoard7Segment = 1;
