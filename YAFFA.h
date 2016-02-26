/******************************************************************************/
/**  YAFFA - Yet Another Forth for Arduino                                   **/
/**                                                                          **/
/**  File: Forth.h                                                           **/
/**  Copyright (C) 2012 Stuart Wood (swood@rochester.rr.com)                 **/
/**                                                                          **/
/**  This file is part of YAFFA.                                             **/
/**                                                                          **/
/**  YAFFA is free software: you can redistribute it and/or modify           **/
/**  it under the terms of the GNU General Public License as published by    **/
/**  the Free Software Foundation, either version 2 of the License, or       **/
/**  (at your option) any later version.                                     **/
/**                                                                          **/
/**  YAFFA is distributed in the hope that it will be useful,                **/
/**  but WITHOUT ANY WARRANTY; without even the implied warranty of          **/
/**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           **/
/**  GNU General Public License for more details.                            **/
/**                                                                          **/
/**  You should have received a copy of the GNU General Public License       **/
/**  along with YAFFA.  If not, see <http://www.gnu.org/licenses/>.          **/
/**                                                                          **/
/******************************************************************************/
/**                                                                          **/
/** Processor Specific Definitions                                           **/
/**                                                                          **/
/** ARDUINO_ARCH_AVR                                                         **/
/**    __AVR_ATmega168__  - Untested                                         **/
/**    __AVR_ATmega168P__ - Untested                                         **/
/**    __AVR_ATmega328P__ - Supported                                        **/
/**    __AVR_ATmega1280__ - Untested                                         **/
/**    __AVR_ATmega2560__ - Untested                                         **/
/**    __AVR_ATmega32U4__ - Supported                                        **/
/**                                                                          **/
/** Board                   Architecture  Flash        SRAM        EEPROM    **/
/** ------                  ------------- ------       ------      -------   **/
/** Uno                     AVR           32K          2K          1K        **/
/** Leonardo                AVR           32K          2.5K        1K        **/
/**                                                                          **/
/******************************************************************************/
#ifndef __YAFFA_H__
#define __YAFFA_H__

/******************************************************************************/
/** Memory Alignment Macros                                                  **/
/******************************************************************************/
#define ALIGN_P(x)  x = (cell_t*)((size_t)(x + 1) & -2)
#define ALIGN(x)  x = ((size_t)(x + 1) & -2)

#define T_SIZE(x) pow( 256, sizeof( x ) ) - 1
#define MAX_OF(type) \
        (((type)(~0LU) > (type)((1LU<<((sizeof(type)<<3)-1)) -1LU)) ? \
        (long unsigned int)(type)(~0LU) : \
        (long unsigned int)(type)((1LU<<((sizeof(type)<<3)-1))-1LU))
#define MIN_OF(type) \
        (((type)(1LU<<((sizeof(type)<<3)-1)) < (type)1) ? \
        (long int)((~0LU)-((1LU<<((sizeof(type)<<3)-1))-1LU)) : \
        0L)

/******************************************************************************/
/** Memory Types                                                             **/
/******************************************************************************/
typedef int16_t cell_t;
typedef uint16_t ucell_t;
typedef int32_t dcell_t;
typedef uint32_t udcell_t;

/******************************************************************************/
/**  Environmental Constants and Name Strings                                **/
/******************************************************************************/
#define CORE          TRUE      // Complete Core Word Set
#define CORE_EXT      FALSE     // Complete Extended Core Word Set
#define FLOORED       TRUE      // Floored Division is default
#define MAX_CHAR      0x7E      // Max. value of any character


/******************************************************************************/
/** CPU / Board Specific                                                           **/
/**   STRING_SIZE - Maximum length of a counted string, in characters        **/
/**   HOLD_SIZE   - Size of the pictured numeric output string buffer, in    **/
/**                 characters                                               **/
/**   PAD_SIZE    - Size of the scratch area pointed to by PAD, in characters**/
/**   RSTACK_SIZE - Size of the return stack, in cells                       **/
/**   STACK_SIZE  - Size of the data stack, in cells                         **/
/**   BUFFER_SIZE - Size of the input text buffer, in characters. The min.   **/
/**                 size for an ANS Forth is 80.                             **/
/**   TOKEN_SIZE  - Size of the token (Word) buffer. Max. length of a        **/
/**                 Definitions names shall is TOKEN_SIZE - 1 characters.    **/
/**   FORTH_SIZE  - Size of Forth Space in bytes                             **/
/******************************************************************************/
#if defined(__AVR_ATmega328P__) // Arduino Uno
  static const unsigned long EPROM_SIZE = 1;
  static const unsigned long SRAM_SIZE = 2;
  #define PROC_STR "ATmega328P"
  #define HOLD_SIZE     31
  #define PAD_SIZE      96
  #define RSTACK_SIZE   16
  #define STACK_SIZE    16
  #define BUFFER_SIZE   96
  #define WORD_SIZE     32

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  // Mega 1280 & 2560
  static const unsigned long EPROM_SIZE = 4;
  static const unsigned long SRAM_SIZE = 8;
  #define PROC_STR "ATmega1280"
  #define HOLD_SIZE     31
  #define PAD_SIZE      127
  #define RSTACK_SIZE   32
  #define STACK_SIZE    32
  #define BUFFER_SIZE   127
  #define WORD_SIZE     32

#elif defined(__AVR_ATmega32U4__) // Arduino Leonardo
  static const unsigned long EPROM_SIZE = 1;
  static const unsigned long SRAM_SIZE = 2.5;
  #define PROC_STR "ATmega32U4"
  #define HOLD_SIZE     31
  #define PAD_SIZE      96
  #define RSTACK_SIZE   16
  #define STACK_SIZE    16
  #define BUFFER_SIZE   96
  #define WORD_SIZE     32

#endif

static const unsigned long FORTH_SIZE    ((SRAM_SIZE*1024*2)/(sizeof(cell_t)*3));
  


/*******************************************************************************/
/**                       Enable Dictionary Word Sets                         **/
/*******************************************************************************/
#define CORE_EXT_SET
#define DOUBLE_SET
#define EXCEPTION_SET
#define LOCALS_SET
#define MEMORY_SET
#define FACILITY_SET
#define TOOLS_SET
#define SEARCH_SET
#define STRING_SET
#define EN_ARDUINO_OPS
#define EN_EEPROM_OPS

/******************************************************************************/
/**  Numbering system                                                        **/
/******************************************************************************/
#define DECIMAL 10
#define HEXIDECIMAL 16
#define OCTAL 8
#define BINARY 2

/******************************************************************************/
/**  ASCII Constants                                                         **/
/******************************************************************************/
#define ASCII_BS    8
#define ASCII_TAB   9
#define ASCII_NL    10
#define ASCII_ESC   27
#define ASCII_CR    13

/******************************************************************************/
/**  Forth True and False                                                    **/
/******************************************************************************/
#define TRUE        -1      // All Bits set to 1
#define FALSE       0       // All Bits set to 0

/******************************************************************************/
/**  Flags - Internal State                                                  **/
/******************************************************************************/
#define ECHO_ON        0x01    // Echo characters typed on the serial input
#define NUM_PROC       0x02    // Pictured Numeric Process
#define EXECUTE        0x04

/******************************************************************************/
/**  wordFlags - Flags used to define word properties                        **/
/******************************************************************************/
// 
#define NORMAL         0x00
#define SMUDGE         0x20    // Word is hidden during searches
#define COMP_ONLY      0x40    // Word is only usable during compilation
#define IMMEDIATE      0x80    // Word is executed during compilation state
#define LENGTH_MASK    0x1F    // Mask for the length of the string

/******************************************************************************/
/**  Control Flow Stack Data Types                                           **/
/******************************************************************************/
#define COLON_SYS    -1
#define DO_SYS       -2
#define CASE_SYS     -3
#define OF_SYS       -4
#define LOOP_SYS     -5


/******************************************************************************/
/**  User Dictionary Header                                                  **/
/******************************************************************************/
typedef struct  {            // structure of the user dictionary
  void*        prevEntry;    // Pointer to the previous entry
  cell_t*      cfa;          // Code Field Address
  uint8_t      flags;        // Holds the length of the following name 
                             // and any flags.
  char         name[];       // Variable length name
} userEntry_t;

/******************************************************************************/
/**  Flash Dictionary Structure                                              **/
/******************************************************************************/
typedef void (*func)(void);         // signature of functions in dictionary

typedef struct  {                   // Structure of the dictionary
  const char*    name;              // Pointer the Word Name in flash
  const func     function;          // Pointer to function
  const uint8_t  flags;             // Holds word type flags
} flashEntry_t;

extern const PROGMEM flashEntry_t flashDict[];        // forward reference

/******************************************************************************/
/**  Flash Dictionary Index References                                       **/
/**  This words referenced here must match the order in the beginning of the **/
/**  dictionary flashDict[]                                                  **/
/******************************************************************************/
#define EXIT_IDX           1
#define LITERAL_IDX        2
#define TYPE_IDX           3
#define JUMP_IDX           4
#define ZJUMP_IDX          5
#define SUBROUTINE_IDX     6
#define THROW_IDX          7
#define DO_SYS_IDX         8
#define LOOP_SYS_IDX       9
#define LEAVE_SYS_IDX      10
#define PLUS_LOOP_SYS_IDX  11
#define EXECUTE_IDX        12
#define S_QUOTE_IDX        13
#define DOT_QUOTE_IDX      14
#define VARIABLE_IDX       15
#define OVER_IDX           16
#define EQUAL_IDX          17
#define DROP_IDX           18

#endif
