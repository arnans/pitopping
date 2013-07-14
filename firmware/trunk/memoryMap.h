//
// memoryMap.c - Defines the memory segments and their addresses
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

#include <eeprom.h>

#define  FLASH32KWORDS

#if defined (__PCM__)
    // Memory Map for PCM (14 bit) parts (i.e. the 16F877 chip)

    //////////////////////////////////////////////////
    //
    //  GoGo Board on-chip FLASH Memory Map (8K)
    //
    //  1FFF  +---------------+
    //        |  Bootloader   |
    //        |               |
    //  1D00  +---------------+
    //        |Logo Procedures|
    //  1C00  +---------------+
    //        |               |
    //        | GoGo Firmware |
    //
    //        |               |
    //  0000  +---------------+
    
    
    
    //////////////////////////////////////////////////
    //   User program memory map
    //   we storm the cricket logo code in the
    //   flash memory to increase the speed.
    //
    //   The size must be limited to 256 bytes because
    //   the boot loader begins at address 0x1d00
    //
    #define FLASH_USER_PROGRAM_BASE_ADDRESS   0x1C00
    
    
    ///////////////////////////////////////////////////
    //  This defines where the bootloader begines
    //  the user program must not exceed this address
    #define FLASH_BOOTLOADER_BASE_ADDRESS     0x1D00
    
    
    //////////////////////////////////////////////////
    //  Location of the run button vector.
    //  This is 0xff0 in the cricket. We map it to
    //  the following address.
    #define RUN_BUTTON_BASE_ADDRESS           0x1CF0


    //////////////////////////////////////////////////
    //
    //  GoGo Board EEPROM chip Memory Map (32K)
    //
    //  7FFF  +---------------+
    //        |               |
    //        |               |
    //        |               |
    //        |  Data Logging |
    //        |               |
    //        |               |
    //        |               |
    //  0500  +---------------+
    //        |               |
    //        | Array Storage |
    //        |               |
    //  0010  +---------------+
    //        | Misc Storage  |
    //  0000  +---------------+
    
    
    
    
    //////////////////////////////////////////////////
    //
    //  The following addresses are locations in the
    //  EEPROM chip
    
    
    /////////////////////////////////////////////////
    //   Data recording memory map
    //   Mem start from 0x0500 and goes to 0x7FFF (~30k)
    #define RECORD_BASE_ADDRESS      0x0500
    
    
    /////////////////////////////////////////////////
    //   Base address for the array memory
    #define ARRAY_BASE_ADDRESS      0x10
    
    
    //////////////////////////////////////////////////
    //  Base address for the memory pointer storage
    //  - Normally this is kept in the variable gblRecordPtr
    //  - But we record this value to the eeprom chip
    //    everytime we record data.
    //  - This way we know where is the most recently
    //    recorded location. It would be useful to know
    //    this in the case of temporary power loss.
    //  - It is used for automatic EEPROM upload
    
    // note that the mem pointer occupies two bytes
    #define MEM_PTR_LOG_BASE_ADDRESS 0x00



// for PIC18 parts with 16K words flash memory. I.e. the 18F458, 18F4550 chips
#elif defined(__PCH__)

   #ifdef FLASH16KWORDS

    //////////////////////////////////////////////////
    //
    //  GoGo Board on-chip FLASH Memory Map (16K Words)
    //
    //  3FFF  +---------------+
    //        |  Bootloader   |
    //  3D00  +---------------+
    //  3CF0  +---------------+ Configuration Bytes (see below)
    //        |Logo Procedures| 1.0K Words
    //        |               |
    //  3900  +---------------+
    //        | Array Storage | 0.5K Words
    //  3700  +---------------+
    //        |               |
    //        |               |
    //        | Data Logging  | ~4 K Words
    //        |               |
    //        |               |
    //  2800  +---------------+ -> check RESERVED_MEMORY_START defined in gogo40.c
    //        |               |
    //        |               |
    //        | GoGo Firmware | 10K Words
    //        |               |
    //        |               |
    //  0000  +---------------+

    // Note that the addresses above are word addresses.
    // PCH parts address memory in bytes which are 2x of
    // the Word addresses

    //  There are 16 configureation words between address
    //  3CF0 and 3D00. Here's the layout.
    //
    //  L,L,L,L,L,L,L,L,D,D,D,D,D,D,D,D
    //  ^               ^
    //  3CF0            3CF8
    //
    //  L = Logo Configurations (i.e. the start button pointer)
    //  D = Data Logger Configurations (i.e. current data location)


    ///////////////////////////////////////////////////
    //  This defines where the bootloader begines
    //  the user program must not exceed this address
    #define FLASH_BOOTLOADER_BASE_ADDRESS     0x7A00


    //////////////////////////////////////////////////
    //  Location of the run button vector.
    //  This is 0xff0 in the cricket. We map it to
    //  the following address.
    #define RUN_BUTTON_BASE_ADDRESS           0x79E0
    

    //////////////////////////////////////////////////
    //  Base address for the memory pointer storage
    //  - Normally this is kept in the variable gblRecordPtr
    //  - But we record this value to the flash mem
    //    everytime we record data.
    //  - This way we know where is the most recently
    //    recorded location. It would be useful to know
    //    this in the case of temporary power loss.
    //  - It is used for automatic Data upload

    // note that the mem pointer occupies two bytes
    #define MEM_PTR_LOG_BASE_ADDRESS 0x79F0

    //////////////////////////////////////////////////
    // Saves  the configuration for Logo procedure
    // autorun. 0=off, 1=on
    // If turned on. Any stored Logo procedure will
    // run automatically everyting the board is
    // turned on.
    #define AUTORUN_STATUS_ADDRESS 0x79F2


    //////////////////////////////////////////////////
    //   User program memory map
    //
    #define FLASH_USER_PROGRAM_BASE_ADDRESS   0x7200


    /////////////////////////////////////////////////
    //   Base address for the array memory
    #define ARRAY_BASE_ADDRESS      0x6E00


    ////////////////////////////////////////////////////////////////
    //   Data recording memory map
    //   Start at RESERVED_MEMORY_START which is defined in gogo40.c
    #define RECORD_BASE_ADDRESS      RESERVED_MEMORY_START


   ////////////////////////////////////////////////////////////////
   //  PIC 18F66J50 (32 Kwords)
   ////////////////////////////////////////////////////////////////
   
   #elif defined(FLASH32KWORDS)
   
    //////////////////////////////////////////////////
    //
    //  GoGo Board on-chip FLASH Memory Map (32K Words)
    //
    //  7FFF  +---------------+
    //        |  Bootloader   |
    //  7D00  +---------------+
    //  7CF0  +---------------+ Configuration Bytes (see below)
    //        |Logo Procedures| ~2.0K Words
    //        |               |
    //  7400  +---------------+
    //        | Array Storage | 0.5K Words
    //  7200  +---------------+
    //        |               |
    //        |               |
    //        | Data Logging  | ~18 K Words
    //        |               |
    //        |               |
    //  2800  +---------------+ -> check RESERVED_MEMORY_START defined in gogo40.c
    //        |               |
    //        |               |
    //        | GoGo Firmware | ~10K Words
    //        |               |
    //        |               |
    //  0000  +---------------+

    // Note that the addresses above are word addresses.
    // PCH parts address memory in bytes which are 2x of
    // the Word addresses

    //  There are 16 configureation words between address
    //  7CF0 and 7D00. Here's the layout.
    //
    //  L,L,L,L,L,L,L,L,D,D,D,D,D,D,D,D
    //  ^               ^
    //  7CF0            7CF8
    //
    //  L = Logo Configurations (i.e. the start button pointer)
    //  D = Data Logger Configurations (i.e. current data location)
    //
    //  L0 = Run button pointer
    //  D0 = Data record pointer
    //  D1 = Autorun flag

    ///////////////////////////////////////////////////
    //  This defines where the bootloader begines
    //  the user program must not exceed this address
    #define FLASH_BOOTLOADER_BASE_ADDRESS     0xFA00


    //////////////////////////////////////////////////
    //  Location of the run button vector.
    //  This is 0xff0 in the cricket. We map it to
    //  the following address.
    #define RUN_BUTTON_BASE_ADDRESS           0xF9E0
    

    //////////////////////////////////////////////////
    //  Base address for the memory pointer storage
    //  - Normally this is kept in the variable gblRecordPtr
    //  - But we record this value to the flash mem
    //    everytime we record data.
    //  - This way we know where is the most recently
    //    recorded location. It would be useful to know
    //    this in the case of temporary power loss.
    //  - It is used for automatic Data upload

    // note that the mem pointer occupies two bytes
    #define MEM_PTR_LOG_BASE_ADDRESS 0xF9F0

    //////////////////////////////////////////////////
    // Saves  the configuration for Logo procedure
    // autorun. 0=off, 1=on
    // If turned on. Any stored Logo procedure will
    // run automatically everyting the board is
    // turned on.
    #define AUTORUN_STATUS_ADDRESS 0xF9F2


    //////////////////////////////////////////////////
    //   User program memory map
    //
    #define FLASH_USER_PROGRAM_BASE_ADDRESS   0xE800


    /////////////////////////////////////////////////
    //   Base address for the array memory
    #define ARRAY_BASE_ADDRESS      0xE400


    ////////////////////////////////////////////////////////////////
    //   Data recording memory map
    //   Start at RESERVED_MEMORY_START which is defined in gogo40.c
    #define RECORD_BASE_ADDRESS      RESERVED_MEMORY_START   

   #endif

#endif






