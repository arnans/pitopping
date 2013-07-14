//
// gogo40.h - the main constant declarations for the firmware
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




#define byte int

#if defined(__PCM__)

#define IOPointer int

#elif defined(__PCH__)

#define IOPointer int16

#endif

/////////////////////////////////////////////////////////


#define CMD_PING         0x00
#define CMD_READ_SENSOR      0x01
#define CMD_MOTOR_CONTROL   0x02
#define CMD_MOTOR_POWER      0x03
#define CMD_TALK_TO_MOTOR   0x04
#define CMD_BURST_MODE      0x05
#define CMD_MISC_CONTROL   0x06

#define TURN_USER_LED_ON      0
#define TURN_USER_LED_OFF     1

#define MISC_USER_LED    0
#define MISC_BEEP        1
#define MISC_SET_PWM     2
#define MISC_UPLOAD_EEPROM    3
#define MISC_I2C_SETUP  4
//#define MISC_I2C_RW     5
#define MISC_AUTORUN_CONFIG   5

#define I2C_START  0
#define I2C_STOP   1
#define I2C_WRITE  2
#define I2C_READ   3

#define AUTORUN_ON   1
#define AUTORUN_OFF  0




#define MTR_ON       0
#define MTR_OFF      1
#define MTR_RD       2
#define MTR_THISWAY  3
#define MTR_THATWAY  4
#define MTR_COAST    5


#define ACK_BYTE       0b10101010   // 0xAA
#define InHeader1       0x54
#define InHeader2       0xfe
#define ReplyHeader1    0x55
#define ReplyHeader2    0xff

#define EEPROMuploadHeader1   0xEE
#define EEPROMuploadHeader2   0x11

#define ON            1
#define OFF            0

// this is used in main(). Determinds how long to wait for the
// second command byte.
#define RETRY         100


//  Sensor read modes
#define NORMAL_READ  0
#define MAX_READ     1
#define MIN_READ     2
#define MEAN_READ    3


// this const defines the last command that is
// one byte long. Please refer to the CMD bits
// in the GoGo kit serial protocol

#define ONE_BYTE_CMD   3


/// How many motors does the board have.
#define MotorCount      4

/// Motor Modes
#define MOTOR_NORMAL    0
#define MOTOR_SERVO     1

// Motor Pin Mappings
                                 // Schematic PIN Names
#define MTR1_CW         PIN_D0   // MD12
#define MTR1_CC         PIN_D1   // MD11
//#define MTR1_EN         PIN_B1   // ME1

#define MTR2_CW         PIN_D2   // MD22
#define MTR2_CC         PIN_D3   // MD21
//#define MTR2_EN         PIN_E6   // ME2

#define MTR3_CW         PIN_B2   // MD32
#define MTR3_CC         PIN_B3   // MD31
//#define MTR3_EN         PIN_C7   // ME3

#define MTR4_CW         PIN_B5   // MD42
#define MTR4_CC         PIN_B4   // ME41
//#define MTR4_EN         PIN_D2   // ME4

//!
//!#define MTR1_CW         PIN_B5   // MD12
//!#define MTR1_CC         PIN_B4   // MD11
//!//#define MTR1_EN         PIN_B1   // ME1
//!
//!#define MTR2_CW         PIN_B2   // MD22
//!#define MTR2_CC         PIN_B3   // MD21
//!//#define MTR2_EN         PIN_E6   // ME2
//!
//!#define MTR3_CW         PIN_D2   // MD32
//!#define MTR3_CC         PIN_D3   // MD31
//!//#define MTR3_EN         PIN_C7   // ME3
//!
//!#define MTR4_CW         PIN_D0   // MD42
//!#define MTR4_CC         PIN_D1   // ME41
//!



#define MOTOR_AB_EN     PIN_E6   // controls the power to the motor chip
#define MOTOR_CD_EN     PIN_B1   // controls the power to the motor chip



#define PIC_TRIS_A   0b00101111  // A0,1,2,3,5 are Analog inputs
#define PIC_TRIS_B   0b00000001  // B0 is the run button
#define PIC_TRIS_C   0b10010000  // C7 = Rs232 Rx
                                 // C4 = I2C SDA
                                 
#define PIC_TRIS_D   0b00000000
#define PIC_TRIS_E   0b10000000  // E7 = IR input
#define PIC_TRIS_F   0b01111100
#define PIC_TRIS_G   0b00000000


#define RUN_BUTTON   PIN_B0
#define USER_LED     PIN_C0
#define USER_LED2    PIN_C1
#define RUN_LED      USER_LED2



// Macros to simplify I/O operations
//

#define USER_LED_ON      output_high(USER_LED)
#define USER_LED_OFF     output_low(USER_LED)


//#define RS232_XMIT      PIN_C6  // (output) RS232 serial transmit
//#define RS232_RCV       PIN_C7  // (input) RS232 serial receive


#define WAITING_FOR_FIRST_HEADER   1
#define WAITING_FOR_SECOND_HEADER   2
#define WAITING_FOR_CMD_BYTE      3
#define WAITING_FOR_SECOND_CMD_BYTE   4
#define CMD_READY               5

#define WAITING_FOR_EXT_CMD_LENGTH  6
#define WAITING_FOR_EXT_CMD_STREAM  7


//=========================================================
// I2C Stuff

#define I2C_SCL            PIN_C3
#define I2C_SDA            PIN_C4

//=========================================================
#define PIEZO              PIN_C2

#define LOCAL              0
#define REMOTE             1



// ========================================================
// Serial Buffer Constants

#define SERIAL_BUFFER_SIZE             32

#define SERIAL_NO_DATA                  0
#define SERIAL_SUCCESS                  1
#define SERIAL_OVERFLOW                 2


// ========================================================
// Display Module Stuff

#define DISPLAY_7SEG_ADDR_1      0xB0
#define DISPLAY_7SEG_ADDR_2      0xB2
#define DISPLAY_LCD_ADDR_1       0xB4
#define DISPLAY_LCD_ADDR_2       0xB6

// ========================================================
// Communication channels 
#define COMM_SERIAL              1
#define COMM_USB                 2

