/******************************************************************************/
/**  YAFFA - Yet Anouther Forth for Adruino                                  **/
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
/**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.         **/
/**                                                                          **/
/******************************************************************************/

/******************************************************************************/
/**  Enviromental Constansts and Name Strings                                **/
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

#define BUFFER_SIZE   96        // Min. size is 80 charicters for ANS Forth
#define TOKEN_SIZE    32        // Definitions names shall contain 1 to 31 char.
#define NAME_SIZE     256       // Size of Name Space in characters
#define CODE_SIZE     192       // Size of Code Space in cells
#define DATA_SIZE     256       // Size of Data Space in characters

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
#define COMP_ONLY      0x40    // Word is only useable during compolation
#define IMMEDIATE      0x80    // Word is executed during compolation state

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
/**  User Dictionary is stored in name space.                                **/
/******************************************************************************/
typedef struct  {               // structure of the user dictionary
  void*          prevEntry;     // Pointer to 'len' field of previous entry
  ucell_t        code;          // SRAM Address for the start of the code
  uint8_t        flags;         // Flags for word types
  char           name[];        // Variable length name
} userEntry_t;

/*****************************************************************************/
/** Function Prototypes                                                     **/
/*****************************************************************************/
uint8_t serial_print_P(const char* ptr);  //prototype of function in Forth.c

