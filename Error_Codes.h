/******************************************************************************/
/**  YAFFA - Yet Another Forth for Arduino                                   **/
/**                                                                          **/
/**  File: Error_Codes.h                                                     **/
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
/******************************************************************************/
/* Error Handling                                                             */
/******************************************************************************/
/**  THROW Code Assignments                                                  **/
/**  -1 ABORT                                                                **/
/**  -2 ABORT"                                                               **/
/**  -3 stack overflow                                                       **/
/**  -4 stack underflow                                                      **/
/**  -5 return stack overflow                                                **/
/**  -6 return stack underflow                                               **/
/**  -7 do-loops nested too deeply during execution                          **/
/**  -8 dictionary overflow                                                  **/
/**  -9 invalid memory address                                               **/
/**  -10 division by zero                                                    **/
/**  -11 result out of range                                                 **/
/**  -12 argument type mismatch                                              **/
/**  -13 undefined word                                                      **/
/**  -14 interpreting a compile-only word                                    **/
/**  -15 invalid FORGET                                                      **/
/**  -16 attempt to use zero-length string as a name                         **/
/**  -17 pictured numeric output string overflow                             **/
/**  -18 parsed string overflow                                              **/
/**  -19 definition name too long                                            **/
/**  -20 write to a read-only location                                       **/
/**  -21 unsupported operation  (e.g., AT-XY on a too-dumb terminal)         **/
/**  -22 control structure mismatch                                          **/
/**  -23 address alignment exception                                         **/
/**  -24 invalid numeric argument                                            **/
/**  -25 return stack imbalance                                              **/
/**  -26 loop parameters unavailable                                         **/
/**  -27 invalid recursion                                                   **/
/**  -28 user interrupt                                                      **/
/**  -29 compiler nesting                                                    **/
/**  -30 obsolescent feature                                                 **/
/**  -31 >BODY used on non-CREATEd definition                                **/
/**  -32 invalid name argument (e.g., TO xxx)                                **/
/**  -33 block read exception                                                **/
/**  -34 block write exception                                               **/
/**  -35 invalid block number                                                **/
/**  -36 invalid file position                                               **/
/**  -37 file I/O exception                                                  **/
/**  -38 non-existent file                                                   **/
/**  -39 unexpected end of file                                              **/
/**  -40 invalid BASE for floating point conversion                          **/
/**  -41 loss of precision                                                   **/
/**  -42 floating-point divide by zero                                       **/
/**  -43 floating-point result out of range                                  **/
/**  -44 floating-point stack overflow                                       **/
/**  -45 floating-point stack underflow                                      **/
/**  -46 floating-point invalid argument                                     **/
/**  -47 compilation word list deleted                                       **/
/**  -48 invalid POSTPONE                                                    **/
/**  -49 search-order overflow                                               **/
/**  -50 search-order underflow                                              **/
/**  -51 compilation word list changed                                       **/
/**  -52 control-flow stack overflow                                         **/
/**  -53 exception stack overflow                                            **/
/**  -54 floating-point underflow                                            **/
/**  -55 floating-point unidentified fault                                   **/
/**  -56 QUIT                                                                **/
/**  -57 exception in sending or receiving a character                       **/
/**  -58 [IF], [ELSE], or [THEN] exception                                   **/

typedef struct {
  const uint8_t code;
  const char* name;
} exception_t;

//extern exception_t exception[];        // forward reference
const char error_1_str[] PROGMEM = "Abort";
const char error_2_str[] PROGMEM = "Abort Quote";
const char error_3_str[] PROGMEM = "Stack Overflow";
const char error_4_str[] PROGMEM = "Stack Underflow";
const char error_5_str[] PROGMEM = "Return Stack Overflow";
const char error_6_str[] PROGMEM = "Return Stack Underflow";
const char error_9_str[] PROGMEM = "Invalid Address";
const char error_10_str[] PROGMEM = "Divide by Zero";
const char error_13_str[] PROGMEM = "Undefined Word";
const char error_14_str[] PROGMEM = "Interpreting a Compile-Only Word";
const char error_16_str[] PROGMEM = "Attempt to use zero-length string as a name";
const char error_18_str[] PROGMEM = "Parsed string overflow";
const char error_22_str[] PROGMEM = "Control Structure Mismatch";
const char error_23_str[] PROGMEM = "Address Alignment Exception";
const char error_31_str[] PROGMEM = ">BODY used on non-CREATEd definition";
const char error_32_str[] PROGMEM = "Invalid Name Argument";
const char error_70_str[] PROGMEM = "Buffer if Full";

const exception_t exception[] PROGMEM = {
  { -1,    error_1_str       },
  { -2,    error_2_str       },
  { -3,    error_3_str       },
  { -4,    error_4_str       },
  { -5,    error_5_str       },
  { -6,    error_6_str       },
  { -9,    error_9_str       },
  { -10,   error_10_str      },
  { -13,   error_13_str      },
  { -14,   error_14_str      },
  { -16,   error_16_str      },
  { -18,   error_18_str      },
  { -22,   error_22_str      },
  { -23,   error_23_str      },
  { -31,   error_31_str      },
  { -32,   error_32_str      },
  { -70,   error_70_str      },
  { 0,     0                 }
};



