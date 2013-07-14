//
// logovm.h - Logo compiler constants and function definitions
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
//    Fred G. Martin
//    Brian Silverman
//    Mitchel Resnick
//    Robbie Berg
//
/////////////////////////////////////////////////////


#include <stack.h>
#include <evalOpcode.h>

#include <memoryMap.h>



/// Logo VM state machine definition

#define IDLE                     0

/// Comm protocol Sates

#define SET_PTR_HI_BYTE          128
#define SET_PTR_LOW_BYTE         129
#define READ_BYTES_COUNT_HI      130
#define READ_BYTES_COUNT_LOW     131
#define WRITE_BYTES_COUNT_HI     132
#define WRITE_BYTES_COUNT_LOW    133
#define WRITE_BYTES_SENDING      134
#define CRICKET_NAME             135



/// Comm commands

#define SET_PTR         0x83
#define READ_BYTES      0x84
#define WRITE_BYTES     0x85
#define RUN             0x86
#define CRICKET_CHECK   0x87






//////////////////////////////////////
//
//   Op code

#define  CODE_END             0
#define  NUM8                 1
#define  NUM16                2
#define  LIST                 3
#define  EOL                  4
#define  EOLR                 5
#define  LTHING               6
#define  STOP                 7
#define  OUTPUT               8
#define  REPEAT               9
#define  COND_IF              10
#define  COND_IFELSE          11
#define  BEEP                 12
#define  NOTE                 13
#define  WAITUNTIL            14
#define  LOOP                 15
#define  WAIT                 16
#define  TIMER                17
#define  RESETT               18
#define  SEND                 19
#define  IR                   20
#define  NEWIR                21
#define  RANDOM               22
#define  OP_PLUS               23
#define  OP_MINUS               24
#define  OP_MULTIPLY            25
#define  OP_DIVISION            26
#define  OP_REMAINDER         27
#define  OP_EQUAL               28
#define  OP_GREATER            29
#define  OP_LESS               30
#define  OP_AND               31
#define  OP_OR                  32
#define  OP_XOR               33
#define  OP_NOT               34
#define  SETGLOBAL            35
#define  GETGLOBAL            36
#define  ASET                  37
#define  AGET                  38
#define  RECORD               39
#define  RECALL               40
#define  RESETDP               41
#define  SETDP                  42
#define  ERASE                  43
#define  WHEN                  44
#define  WHENOFF               45
#define  M_A                  46
#define  M_B                  47
#define  M_AB                  48
#define  M_ON                  49
#define  M_ONFOR               50
#define  M_OFF                  51
#define  M_THISWAY            52
#define  M_THATWAY            53
#define  M_RD                  54
#define  SENSOR1               55
#define  SENSOR2               56
#define  SWITCH1               57
#define  SWITCH2               58
#define  SETPOWER               59
#define  BRAKE                  60
#define  BSEND                  61
#define  BSR                  62
#define  M_C                  63
#define  M_D                  64
#define  M_CD                  65
#define  M_ABCD               66
#define  FASTSEND               67
#define  REALLY_STOP          68
#define  EB                     69
#define  DB                     70
#define  LOW_BYTE               71
#define  HIGH_BYTE            72

/// These code are unique to the GoGo board
#define  SENSOR3              73
#define  SENSOR4              74
#define  SENSOR5              75
#define  SENSOR6              76
#define  SENSOR7              77
#define  SENSOR8              78
#define  SWITCH3              79
#define  SWITCH4              80
#define  SWITCH5              81
#define  SWITCH6              82
#define  SWITCH7              83
#define  SWITCH8              84

#define ULED_ON               85
#define ULED_OFF              86

#define SERVO_SET_H           87
#define SERVO_LT              88
#define SERVO_RT              89

#define TALK_TO_MOTOR        90   // this replaces the M_A, M_B, M_C, ... commands.
                           // it will allow a more flexible motor addressing
                           // i.e. abc, would now work.
                           // Only the GoGo Compiler (not Jackal/Logo Blocks)
                           // will make use of this.

#define CL_I2C_START          91
#define CL_I2C_STOP           92
#define CL_I2C_WRITE          93
#define CL_I2C_READ           94

// serial communication commands
#define SERIAL                95
#define NEWSERIAL             96

// DS1307 Realtime clock module commands
#define RTC_INIT              97
#define RTC_GET_ITEM          98

// 7-segment/LCD display module commands
#define DISPLAY_SHOW             99

#define DISPLAY_CLS           100 
#define DISPLAY_GETPOS        101 // does nothing on 7-segment displays
#define DISPLAY_SETPOS        102 // returns 0 on 7-segment displays
#define TALK_TO_7SEG_1        103
#define TALK_TO_7SEG_2        104
#define TALK_TO_LCD_1         105
#define TALK_TO_LCD_2         106



/////////////////////////////////////////////////
//   Global variables


int16 globalVariables[16]={0};

unsigned int16 gblRecordPtr; // pointer to the current location in the data eeprom
                             // it will be initiazlied to the most recent record location
                             // in init_variables()

unsigned int16  gblMemPtr,     // FLASH/EEPROM pointer
                gblRWCount;    // Read/Write length

// Sends bytes to the computer
void sendBytes(unsigned int16 memPtr, unsigned int16 count);
unsigned char fetchNextOpcode();
