/******************************************************************************/
/**  YAFFA - Yet Anouther Forth for Adruino                                  **/
/**                                                                          **/
/**  File: Dictionary.h                                                      **/
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
#define EXIT_IDX           1
#define LITERAL_IDX        2
#define TYPE_IDX           3
#define JUMP_IDX           4
#define ZJUMP_IDX          5
#define NZJUMP_IDX         6
#define SUBROUTINE_IDX     7
#define THROW_IDX          8
#define DO_SYS_IDX         9
#define LOOP_SYS_IDX       10
#define LEAVE_SYS_IDX      11
#define PLUS_LOOP_SYS_IDX  12
#define EXECUTE_IDX        13

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
/**  Flash Dictionary Structure                                              **/
/******************************************************************************/
typedef void (*func)(void);         // signature of functions in dictionary

typedef struct  {                   // Structure of the dictionary
  const char*    name;              // Pointer the Word Name in flash
  const func     function;          // Pointer to function
  const uint8_t  flags;             // Holds word type flags
} flashEntry_t;

extern flashEntry_t flashDict[];        // forward reference


