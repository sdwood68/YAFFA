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
/** Architecture Specific Definitions                                        **/
/**                                                                          **/
/** ARDUINO_ARCH_AVR                                                         **/
/**    __AVR_ATmega168__  - Untested                                         **/
/**    __AVR_ATmega168P__ - Untested                                         **/
/**    __AVR_ATmega328P__ - Supported                                        **/
/**    __AVR_ATmega1280__ - Untested                                         **/
/**    __AVR_ATmega2560__ - Untested                                         **/
/**    __AVR_ATmega32U4__ - Supported                                        **/
/**                                                                          **/
/** ARDUINO_ARCH_SAMD - Unsupported                                          **/
/**    __ARDUINO_SAMD_ZERO__ - Unsupported                                   **/
/**                                                                          **/
/**                                                                          **/
/** Board        Architecture             Flash        SRAM        EEPROM    **/
/** ------       -------------            ------       ------      -------   **/
/** Uno          AVR                      32K          2K          1K        **/
/** Leonardo     AVR                      32K          2.5K        1K        **/
/** Zero         SAMD                     256K         32K         -         **/
/**                                                                          **/
/******************************************************************************/
#ifndef __YAFFA_H__
#define __YAFFA_H__

/******************************************************************************/
/** Memory Alignment Macros                                                  **/
/******************************************************************************/
#define ALIGN_P(x)  x = (uint8_t*)((addr_t)(x + 1) & -2)
#define ALIGN(x)  x = ((addr_t)(x + 1) & -2)

/******************************************************************************/
/** Architecture Specific                                                    **/
/******************************************************************************/
#if defined(ARDUINO_ARCH_AVR)    // 8 bit Processor
  #define ARCH_STR "Atmel AVR"
  #define CELL_BITS 16
  #define ADDRESS_BITS 16
  typedef int16_t cell_t;
  typedef uint16_t ucell_t;
  typedef int32_t dcell_t;
  typedef uint32_t udcell_t;
  typedef uint16_t addr_t;

#elif defined(ARDUINO_ARCH_SAMD) // 32 bit Processor
  #define ARCH_STR "Atmel SAMD"
  #define CELL_BITS 32 
  #define ADDRESS_BITS 32
  #define PROGMEM
  #define strcasecmp_P(...) strcasecmp(__VA_ARGS__) 
  #define strchr_P(...) strchr(__VA_ARGS__)
  typedef int32_t cell_t;
  typedef uint32_t ucell_t;
  typedef int64_t dcell_t;
  typedef uint64_t udcell_t;
  typedef uint32_t addr_t;
#endif

/******************************************************************************/
/**  Environmental Constants and Name Strings                                **/
/******************************************************************************/
#define FLOORED       TRUE      // Floored Division is default
#define MAX_CHAR      0x7E      // Max. value of any character
#define MAX_U         2^(CELL_BITS)-1      // largest usable unsigned integer
#define MAX_N         2^(CELL_BITS-1)-1    // largest usable signed integer
#define MAX_D         2^(2*CELL_BITS-1)-1  // Largest usable signed double number
#define MAX_UD        2^(2*CELL_BITS)-1    // Largest usable unsigned double number


/******************************************************************************/
/** Board Specific                                                           **/
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
  #define STRING_SIZE   31
  #define HOLD_SIZE     31
//  #define PAD_SIZE      31
  #define RSTACK_SIZE   16
  #define STACK_SIZE    16
  #define BUFFER_SIZE   96
  #define WORD_SIZE    32
  #define FORTH_SIZE    1280

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  // Mega 1280 & 2560

#elif defined(__AVR_ATmega32U4__) // Arduino Leonardo
  #define STRING_SIZE   31
  #define HOLD_SIZE     31
//  #define PAD_SIZE      31
  #define RSTACK_SIZE   16
  #define STACK_SIZE    16
  #define BUFFER_SIZE   96
  #define WORD_SIZE    32
  #define FORTH_SIZE    1580

#elif defined(__ARDUINO_SAMD_ZERO__) 
  #define STRING_SIZE   256
  #define HOLD_SIZE     64
  #define PAD_SIZE      128
  #define RSTACK_SIZE   32
  #define STACK_SIZE    32
  #define BUFFER_SIZE   256
  #define TOKEN_SIZE    32
  #define FORTH_SIZE    28*1024

#endif


/*******************************************************************************/
/**                       Enable Dictionary Word Sets                         **/
/*******************************************************************************/
#define CORE_EXT_SET
//#define DOUBLE_SET
#define EXCEPTION_SET
//#define LOCALS_SET
//#define MEMORY_SET
#define TOOLS_SET
//#define SEARCH_SET
//#define STRING_SET
#define EN_ARDUINO_OPS
#if defined(ARDUINO_ARCH_AVR)
#define EN_EEPROM_OPS
#endif

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
/**  Flags - Internal State and Word                                         **/
/******************************************************************************/
// Flags used to define word properties 
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
  addr_t       prevEntry;    // Pointer to the previous entry
  addr_t       cfa;          // Code Field Address
//  addr_t       pfa;          // Parameter Field Address
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

#endif
