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
#define EN_EEPROM_OPS

/******************************************************************************/
/**  Environmental Constants and Name Strings                                **/
/******************************************************************************/
#define STRING_SIZE   31        // Maximum size of a counted string, in
                                // characters
#define HOLD_SIZE     31        // Size of the pictured numeric output string 
                                // buffer, in characters
#define PAD_SIZE      31        // Size of the scratch area pointed to by PAD,
                                // in characters
#define FLOORED       TRUE      // Floored Division is default
#define ADDRESS_BITS  16        // Size of one address unit, in bits
#define MAX_CHAR      0x7E      // Max. value of any character
#define MAX_D         2^31-1    // Largest usable signed double number
#define MAX_N         2^15-1    // largest usable signed integer
#define MAX_U         2^16-1    // largest usable unsigned integer
#define MAX_UD        2^32-1    // Largest usable unsigned double number
#define RSTACK_SIZE   16        // Max. size of the return stack, in cells
#define STACK_SIZE    16        // Max. size of the data stack, in cells

#define BUFFER_SIZE   96        // Min. size is 80 characters for ANS Forth
#define TOKEN_SIZE    32        // Definitions names shall contain 1 to 31 char.
#define FORTH_SIZE   1280      // Size of Forth Space in bytes

/******************************************************************************/
/**  Forth True and False                                                    **/
/******************************************************************************/
#define TRUE           -1      // All Bits set to 1
#define FALSE          0       // All Bits set to 0

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
/**  Cell Definitions & Stacks                                               **/
/******************************************************************************/
typedef int16_t cell_t;
typedef uint16_t ucell_t;
typedef int32_t dcell_t;
typedef uint32_t udcell_t;
typedef uint16_t addr_t;

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

/*****************************************************************************/
/** Function Prototypes                                                     **/
/*****************************************************************************/
//uint8_t serial_print_P(const char* ptr);  //prototype of function in Forth.c

