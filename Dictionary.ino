/******************************************************************************/
/**  YAFFA - Yet Another Forth for Arduino                                   **/
/**                                                                          **/
/**  File: Dictionary.ino                                                    **/
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
const char not_done_str[] PROGMEM = " NOT Implemented Yet \n\r";

/******************************************************************************/
/**                       Primitives for Control Flow                        **/
/******************************************************************************/
const PROGMEM char jump_str[] = "jump";
static void _jump(void) {
  ip = (cell_t*)((cell_t)ip + *ip);
#ifdef DEBUG
  debugNewIP();
#endif
}

const PROGMEM char zjump_str[] = "zjump";
static void _zjump(void) {
#ifdef DEBUG
  debugValue(ip);
#endif
  if(!pop()) ip = (cell_t*)((cell_t)ip + *ip);
  else ip++;
#ifdef DEBUG
  debugNewIP();
#endif
}

const PROGMEM char subroutine_str[] = "subroutine";
static void _subroutine(void) {
  *pDoes = (cell_t)*ip++;
#ifdef DEBUG
  debugValue(pDoes);
#endif
}

const PROGMEM char do_sys_str[] = "do-sys";
// ( n1|u1 n2|u2 -- ) (R: -- loop_sys )
// Set up loop control parameters with index n2|u2 and limit n1|u1. An ambiguous
// condition exists if n1|u1 and n2|u2 are not the same type. Anything already 
// on the return stack becomes unavailable until the loop-control parameters 
// are discarded.
static void _do_sys(void) {
  rPush(LOOP_SYS);
  rPush(pop());   // push index on to return stack
  rPush(pop());   // push limit on to return stack
}

const PROGMEM char loop_sys_str[] = "loop-sys";
// ( n1|u1 n2|u2 -- ) (R: -- loop_sys )
// Set up loop control parameters with index n2|u2 and limit n1|u1. An ambiguous
// condition exists if n1|u1 and n2|u2 are not the same type. Anything already 
// on the return stack becomes unavailable until the loop-control parameters 
// are discarded.
static void _loop_sys(void) {
  cell_t limit = rPop();    // fetch limit
  cell_t index = rPop();    // fetch index
  index++;
  if(limit - index) {
    rPush(index);
    rPush(limit);
    ip = (cell_t*)*ip;
  } else {
    ip++;
    if(rPop() != LOOP_SYS) {
      push(-22);
      _throw();
      return;
    }
  }
}

const PROGMEM char leave_sys_str[] = "leave-sys";
// ( -- ) (R: loop-sys -- )
// Discard the current loop control parameters. An ambiguous condition exists 
// if they are unavailable. Continue execution immediately following the 
// innermost syntactically enclosing DO ... LOOP or DO ... +LOOP.
static void _leave_sys(void) {
  rPop();    // fetch limit
  rPop();    // fetch index
  if(rPop() != LOOP_SYS) {
    push(-22);
    _throw();
    return;
  }
  ip = (cell_t*)*ip;
#ifdef DEBUG
  debugNewIP();
#endif
}

const PROGMEM char plus_loop_sys_str[] = "plus_loop-sys";
// ( n1|u1 n2|u2 -- ) (R: -- loop_sys )
// Set up loop control parameters with index n2|u2 and limit n1|u1. An ambiguous
// condition exists if n1|u1 and n2|u2 are not the same type. Anything already 
// on the return stack becomes unavailable until the loop-control parameters 
// are discarded.
static void _plus_loop_sys(void) {
  cell_t limit = rPop();    // fetch limit
  cell_t index = rPop();    // fetch index
  index += pop();
  if(limit != index) {
    rPush(index);
    rPush(limit);
    ip = (cell_t*)*ip;
  } else {
    ip++;
    if(rPop() != LOOP_SYS) {
      push(-22);
      _throw();
      return;
    }
  }
}

/*******************************************************************************/
/**                          Core Forth Words                                 **/
/*******************************************************************************/
const PROGMEM char store_str[] = "!";
// ( x a-addr --)
// Store x at a-addr
static void _store(void) { 
  addr_t address = pop();
  *((cell_t*) address) = pop();
}

const PROGMEM char number_sign_str[] = "#";
// ( ud1 -- ud2)
// Divide ud1 by number in BASE giving quotient ud2 and remainder n. Convert
// n to external form and add the resulting character to the beginning of the
// pictured numeric output string.
static void _number_sign(void) { 
  udcell_t ud;
  ud = (udcell_t)pop()<<sizeof(ucell_t)*8;
  ud += (udcell_t)pop();
#ifdef DEBUG
  serial_print_P(PSTR("  ud = "));
  Serial.println(ud);
  Serial.println(sizeof(ucell_t));
#endif
  *--pPNO = pgm_read_byte(&charset[ud % base]);
  ud /= base;
#ifdef DEBUG
  serial_print_P(PSTR("  new ud = "));
  Serial.println(ud);
  serial_print_P(PSTR("  pPNO = $"));
  Serial.print((addr_t)pPNO, HEX);
  serial_print_P(PSTR(" = "));
  Serial.println((char)*pPNO);
#endif
  push((ucell_t)ud);
  push((ucell_t)(ud >> sizeof(ucell_t)*8));
}

const PROGMEM char number_sign_gt_str[] = "#>";
// ( xd -- c-addr u)
// Drop xd. Make the pictured numeric output string available as a character 
// string c-addr and u specify the resulting string. A program may replace 
// characters within the string.
static void _number_sign_gt(void) {
  _two_drop(); 
  push((cell_t)pPNO);
  push((cell_t)strlen(pPNO));
  flags &= ~NUM_PROC;
}

const PROGMEM char number_sign_s_str[] = "#s";
// ( ud1 -- ud2)
static void _number_sign_s(void) { 
  udcell_t ud;
  ud = (udcell_t)pop() << sizeof(ucell_t)*8;
  ud += (udcell_t)pop();
  while (ud) {
      *--pPNO = pgm_read_byte(&charset[ud % base]);
      ud /= base;
  }
#ifdef DEBUG
  serial_print_P(PSTR("  ud = "));
  Serial.println(ud);
  serial_print_P(PSTR("  pPNO = $"));
  Serial.print((addr_t)pPNO, HEX);
  serial_print_P(PSTR(" = "));
  Serial.println((char)*pPNO);
#endif
  push((ucell_t)ud);
  push((ucell_t)(ud >> sizeof(ucell_t)*8));
}

const PROGMEM char tick_str[] = "'";
// ( "<space>name" -- xt)
// Skip leading space delimiters. Parse name delimited by a space. Find name and
// return xt, the execution token for name. An ambiguous condition exists if 
// name is not found. When interpreting "' xyz EXECUTE" is equivalent to xyz.
static void _tick(void) { 
    push(' ');
    _word();
    _find();
    pop();
//  if (getToken()) {
//    if (isWord(cTokenBuffer)) {
//      push(w);
//      return;
//    }
//  }
//  push(-16);
//  _throw();
}

const PROGMEM char paren_str[] = "(";
// ( "ccc<paren>" -- )
// imedeate
static void _paren(void) { 
  push(')');
  _word();
  _drop();
}

const PROGMEM char star_str[] = "*";
// ( n1|u1 n2|u2 -- n3|u3 )
// multiply n1|u1 by n2|u2 giving the product n3|u3
static void _star(void) {
  push(pop() * pop()); 
}

const PROGMEM char star_slash_str[] = "*/";
// ( n1 n2 n3 -- n4 )
// multiply n1 by n2 producing the double cell result d. Divide d by n3
// giving the single-cell quotient n4.
static void _star_slash(void) {
  cell_t n3 = pop();
  cell_t n2 = pop();
  cell_t n1 = pop();
  dcell_t d = (dcell_t)n1 * (dcell_t)n2;
  push((cell_t)(d / n3)); 
}

const PROGMEM char star_slash_mod_str[] = "*/mod";
// ( n1 n2 n3 -- n4 n5)
// multiply n1 by n2 producing the double cell result d. Divide d by n3
// giving the single-cell remainder n4 and quotient n5.
static void _star_slash_mod(void) {
  cell_t n3 = pop();
  cell_t n2 = pop();
  cell_t n1 = pop();
  dcell_t d = (dcell_t)n1 * (dcell_t)n2;
  push((cell_t)(d % n3)); 
  push((cell_t)(d / n3)); 
}

const PROGMEM char plus_str[] = "+";
// ( n1|u1 n2|u2 -- n3|u3 )
// add n2|u2 to n1|u1, giving the sum n3|u3
static void _plus(void) { 
  cell_t x = pop();
  cell_t y = pop();
  push(x +  y);
}

const PROGMEM char plus_store_str[] = "+!";
// ( n|u a-addr -- )
// add n|u to the single cell number at a-addr
static void _plus_store(void) { 
  addr_t address = pop();
  if (address >= (addr_t)&forthSpace[0] && 
      address < (addr_t)&forthSpace[FORTH_SIZE])
    *((unsigned char*) address) += pop();
  else {
    push(-9);
    _throw();
  }
}

const PROGMEM char plus_loop_str[] = "+loop";
// Interpretation: Interpretation semantics for this word are undefined.
// Compilation: (C: do-sys -- )
// Append the run-time semantics given below to the current definition. Resolve
// the destination of all unresolved occurrences of LEAVE between the location 
// given by do-sys and the next location for a transfer of control, to execute
// the words following +LOOP.
// Run-Time: ( n -- )(R: loop-sys1 -- | loop-sys2 )
// An ambiguous condition exists if the loop control parameters are unavailable. 
// Add n to the index. If the loop index did not cross the boundary between the 
// loop limit minus one and the loop limit, continue execution at the beginning
// of the loop. Otherwise, discard the current loop control parameters and 
// continue execution immediately following the loop.
static void _plus_loop(void) { 
  *(cell_t*)pHere = PLUS_LOOP_SYS_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  *(cell_t*)pHere = pop();
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  cell_t* leave = (cell_t*)pop();
  if (leave != (cell_t*)DO_SYS) {
    if (stack[tos] == DO_SYS) {
      *leave = (ucell_t)pHere;
      pop();
    } else {
      push(-22);
      _throw();
      return;
    }
  }
}

const PROGMEM char comma_str[] = ",";
// ( x --  )
// Reserve one cell of data space and store x in the cell. If the data-space
// pointer is aligned when , begins execution, it will remain aligned when ,
// finishes execution. An ambiguous condition exists if the data-space pointer
// is not aligned prior to execution of ,.
static void _comma(void) { 
//  if (((cell_t)pHere & 1) == 0) {
    *(cell_t*)pHere = pop();
    pHere += sizeof(cell_t);
//  } else {
//    push(-23);
//    _throw();
//  }
}

const PROGMEM char minus_str[] = "-";
// ( n1|u1 n2|u2 -- n3|u3 )
static void _minus(void) {
  cell_t temp = pop();
  push(pop() -  temp);
}

const PROGMEM char dot_str[] = ".";
// ( n -- )
// display n in free field format
static void _dot(void) { 
  w = pop();
  displayValue();
}

const PROGMEM char dot_quote_str[] = ".\x22"; 
// Compilation ("ccc<quote>" -- )
// Parse ccc delimited by ". Append the run time semantics given below to 
// the current definition.
// Run-Time ( -- )
// Display ccc. 
static void _dot_quote(void) {
  uint8_t i;
  char length;
  if (flags & EXECUTE) {
    Serial.print((char*)ip);
    cell_t len = strlen((char*)ip) + 1;  // include null terminator
    ALIGN(len);
    ip = (cell_t*)((cell_t)ip + len);
  }
  else if (state) {
    cDelimiter = '"';
    if(!getToken()) {
      push(-16);
      _throw();
    }
    length = strlen(cTokenBuffer);
    *(cell_t*)pHere = (cell_t)DOT_QUOTE_IDX;
#ifdef DEBUG
    debugXT(pHere);
#endif
    pHere += sizeof(cell_t);
#ifdef DEBUG
    serial_print_P(PSTR("\r\n  String @ $"));
    char* str = (char*)pHere;
    Serial.print((ucell_t)str, HEX);
#endif
    for (uint8_t i = 0; i < length; i++) {
      *(char*)pHere++ = cTokenBuffer[i];
    }
    *(char*)pHere++ = NULL;    // Terminate String
#ifdef DEBUG
    serial_print_P(PSTR(": "));
    Serial.print(str);
#endif
    ALIGN_P(pHere);  // re- align the pHere for any new code
    cDelimiter = ' ';
  }
}

const PROGMEM char slash_str[] = "/";
// ( n1 n2 -- n3 )
// divide n1 by n2 giving a single cell quotient n3
static void _slash(void) { 
  cell_t temp = pop();
  if (temp) 
    push(pop() /  temp);
  else {
    push(-10);
    _throw();
  }
}

const PROGMEM char slash_mod_str[] = "/mod";
// ( n1 n2 -- n3 n4)
// divide n1 by n2 giving a single cell remainder n3 and quotient n4
static void _slash_mod(void) { 
  cell_t n2 = pop();
  cell_t n1 = pop();
  if (n2) {
    push(n1 %  n2);  
    push(n1 /  n2);  
  } else {
    push(-10);
    _throw();
  }
}

const PROGMEM char zero_less_str[] = "0<";
// ( n -- flag )
// flag is true if and only if n is less than zero.
static void _zero_less(void) {
  if (pop() < 0) push(TRUE);
  else push(FALSE);
}

const PROGMEM char zero_equal_str[] = "0=";
// ( n -- flag )
// flag is true if and only if n is equal to zero.
static void _zero_equal(void) {
  if (pop() == 0) push(TRUE);
  else push(FALSE);
}

const PROGMEM char one_plus_str[] = "1+";
// ( n1|u1 -- n2|u2 )
// add one to n1|u1 giving sum n2|u2.
static void _one_plus(void) { 
  push(pop() + 1);
}

const PROGMEM char one_minus_str[] = "1-";
// ( n1|u1 -- n2|u2 )
// subtract one to n1|u1 giving sum n2|u2.
static void _one_minus(void) { 
  push(pop() - 1);
}

const PROGMEM char two_store_str[] = "2!";
// ( x1 x2 a-addr --)
// Store the cell pair x1 x2 at a-addr, with x2 at a-addr and x1 at a-addr+1
static void _two_store(void) { 
  addr_t address = pop();
  if (address >= (addr_t)&forthSpace[0] && 
      address < (addr_t)&forthSpace[FORTH_SIZE - 4]) {
    *(cell_t*)address++ = pop();
    *(cell_t*)address = pop();
  } else {
    push(-9);
    _throw();
  }
}

const PROGMEM char two_star_str[] = "2*";
// ( x1 -- x2 )
// x2 is the result of shifting x1 one bit to toward the MSB
static void _two_star(void) { 
  push(pop() << 1);
}

const PROGMEM char two_slash_str[] = "2/";
// ( x1 -- x2 )
// x2 is the result of shifting x1 one bit to toward the LSB
static void _two_slash(void) { 
  push(pop() >> 1);
}

const PROGMEM char two_fetch_str[] = "2@";  // \x40 == '@'
// ( a-addr -- x1 x2 )
// Fetch cell pair x1 x2 at a-addr. x2 is at a-addr, and x1 is at a-addr+1
static void _two_fetch(void) { 
  addr_t address = pop();
  cell_t value = *(unsigned char*)address;
  push(value);
  address += sizeof(cell_t);
  value = *(unsigned char*)address;
  push(value);
}

const PROGMEM char two_drop_str[] = "2drop";
// ( x1 x2 -- )
static void _two_drop(void) { 
    pop();
    pop();
}

const PROGMEM char two_dup_str[] = "2dup";
// ( x1 x2 -- x1 x2 x1 x2 )
static void _two_dup(void) {
    push(stack[tos - 1]);
    push(stack[tos - 1]);
}

const PROGMEM char two_over_str[] = "2over";
// ( x1 x2 x3 x4 -- x1 x2 x3 x4 x1 x2 )
static void _two_over(void) {
    push(stack[tos - 3]);
    push(stack[tos - 2]);
}

const PROGMEM char two_swap_str[] = "2swap";
// ( x1 x2 x3 x4 -- x3 x4 x1 x2 )
static void _two_swap(void) {
  cell_t x4 = pop();
  cell_t x3 = pop();
  cell_t x2 = pop();
  cell_t x1 = pop();
  push(x3);
  push(x4);
  push(x1);
  push(x2);
}

const PROGMEM char colon_str[] = ":";
// (C: "<space>name" -- colon-sys )
// Skip leading space delimiters. Parse name delimited by a space. Create a 
// definition for name, called a "colon definition" Enter compilation state
// and start the current definition, producing a colon-sys. Append the 
// initiation semantics given below to the current definition....
static void _colon(void) {
  state = TRUE;
  push(COLON_SYS);
  openEntry();
}

const PROGMEM char semicolon_str[] = ";";
// IMMEDIATE
// Interpretation: undefined
// Compilation: (C: colon-sys -- )
// Run-time: ( -- ) (R: nest-sys -- )
static void _semicolon(void) {
  if (pop() != COLON_SYS) {
    push(-22);
    _throw();
    return;
  }
  closeEntry();
  state = FALSE;
}

const PROGMEM char lt_str[] = "<";
// ( n1 n2 -- flag )
static void _lt(void) { 
  if (pop() > pop()) push(TRUE); 
  else push(FALSE);  
}

const PROGMEM char lt_number_sign_str[] = "<#";
// ( -- )
// Initialize the pictured numeric output conversion process.
static void _lt_number_sign(void) { 
  pPNO = (char*)pHere + HOLD_SIZE + 1;
  *pPNO = NULL;
#ifdef DEBUG
  Serial.print("pPNO = $");
  Serial.println((addr_t)pPNO, HEX);
#endif
  flags |= NUM_PROC;
}

const PROGMEM char eq_str[] = "=";
// ( x1 x2 -- flag )
// flag is true if and only if x1 is bit for bit the same as x2
static void _eq(void) {
  if (pop() == pop()) push(TRUE);
  else push(FALSE);
}

const PROGMEM char gt_str[] = ">";
// ( n1 n2 -- flag )
// flag is true if and only if n1 is greater than n2
static void _gt(void) {
  if (pop() < pop()) push(TRUE);
  else push(FALSE);  
}

const PROGMEM char to_body_str[] = ">body";
// ( xt -- a-addr )
// a-addr is the data-field address corresponding to xt. An ambiguous condition
// exists if xt is not for a word defined by CREATE.
static void _to_body(void) {
  cell_t* xt = (cell_t*)pop();
  if ((cell_t)xt > 0xFF) {
    if (*xt++ == LITERAL_IDX) {
      push(*xt);
      return;
    }
  }
  push(-31);
  _throw();
}

const PROGMEM char to_in_str[] = ">in";
// ( -- a-addr )
static void _to_in(void) {
  push((cell_t)&cpToIn); 
}

const PROGMEM char to_number_str[] = ">number";
// ( ud1 c-addr1 u1 -- ud2 c-addr u2 )
static void _to_number(void) {
  serial_print_P(not_done_str); 
}

const PROGMEM char to_r_str[] = ">r";
// ( x -- ) (R: -- x )
static void _to_r(void) {
#ifdef DEBUG
  cell_t temp = pop();
  serial_print_P(PSTR("  Moving $"));
  Serial.print(temp, HEX);
  serial_print_P(PSTR(" To Return Stack\r\n"));
  rPush(temp);
#else
  rPush(pop());
#endif
}

const PROGMEM char question_dup_str[] = "?dup";
// ( x -- 0 | x x )
static void _question_dup(void) {
  if (stack[tos]) {
    push(stack[tos]);
  } else {
    pop();
    push(0);
  }
}

const PROGMEM char fetch_str[] = "@";
// ( a-addr -- x1 )
// Fetch cell x1 at a-addr. 
static void _fetch(void) { 
  addr_t address = pop();
//  if ((address & 1) == 0) {
    cell_t value = *(cell_t*)address;
    push(value);
//  } else {
//    push(-23);
//    _throw();
//  }
}

const PROGMEM char abort_str[] = "abort";
// (i*x -- ) (R: j*x -- )
// Empty the data stack and preform the function of QUIT, which includes emptying
// the return stack, without displaying a message.
static void _abort(void) {
  push(-1);
  _throw();
}

const PROGMEM char abort_quote_str[] = "abort\x22";
// Interpretation: Interpretation semantics for this word are undefined.
// Compilation: ( "ccc<quote>" -- )
// Parse ccc delimited by a ". Append the run-time semantics given below to the
// current definition.
// Runt-Time: (i*x x1 -- | i*x ) (R: j*x -- |j*x )
// Remove x1 from the stack. If any bit of x1 is not zero, display ccc and 
// preform an implementation-defined abort sequence that included the function
// of ABORT.
static void _abort_quote(void) {
  *(cell_t*)pHere = ZJUMP_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  push((cell_t)pHere);  // Push the address for our origin
  *(cell_t*)pHere = 0;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  _dot_quote();
  *(cell_t*)pHere = LITERAL_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  *(cell_t*)pHere = -2;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  *(cell_t*)pHere = THROW_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  cell_t* orig = (cell_t*)pop();
  *orig = (cell_t)pHere - (cell_t)orig;
#ifdef DEBUG
  debugValue(pHere);
#endif
}

const PROGMEM char abs_str[] = "abs";
// ( n -- u)
// Runt-Time: 
static void _abs(void) {
  cell_t n = pop();
  push(n < 0 ? 0 - n : n);
}

const PROGMEM char accept_str[] = "accept";
// ( c-addr +n1 -- +n2 )
static void _accept(void) {
  cell_t length = pop(); 
  char* addr = (char*)pop();
  length = getLine(addr, length);
  push(length);
}

const PROGMEM char align_str[] = "align";
// ( -- )
// if the data-space pointer is not aligned, reserve enough space to align it.
static void _align(void) {
  ALIGN_P(pHere);
}

const PROGMEM char aligned_str[] = "aligned";
// ( addr -- a-addr)
static void _aligned(void) {
  push((pop() + 1) & -2);
}

const PROGMEM char allot_str[] = "allot";
// ( n -- )
// if n is greater than zero, reserve n address units of data space. if n is less
// than zero, release |n| address units of data space. If n is zero, leave the 
// data-space pointer unchanged.
static void _allot(void) {
  uint8_t* pNewHere = pHere + pop();
  // Check that the new pHere is not outside of the forth space
  if (pNewHere >= &forthSpace[0] &&
      pNewHere < &forthSpace[FORTH_SIZE]) {
    pHere = pNewHere;      // Save the valid address
  } else {                 // Throw an exception 
    push(-9);
    _throw();
  }
}

const PROGMEM char and_str[] = "and";
// ( x1 x2 -- x3 )
// x3 is the bit by bit logical and of x1 with x2
static void _and(void) {
  push(pop() & pop());
}

const PROGMEM char base_str[] = "base";
// ( -- a-addr)
static void _base(void) {
  push((cell_t)&base);
}

const PROGMEM char begin_str[] = "begin";
// Interpretation: Interpretation semantics for this word are undefined.
// Compilation: (C: -- dest )
// Put the next location for a transfer of control, dest, onto the control flow
// stack. Append the run-time semantics given below to the current definition.
// Run-time: ( -- )
// Continue execution.
static void _begin(void) {
  push((cell_t)pHere);
  *(cell_t*)pHere = 0;
}

const PROGMEM char bl_str[] = "bl";
// ( -- char )
// char is the character value for a space.
static void _bl(void) {
  push(' ');
}

const PROGMEM char c_store_str[] = "c!";
// ( char c-addr -- )
static void _c_store(void) {
  volatile uint8_t* address = (uint8_t*)pop();
  *address = (uint8_t)pop();
}

const PROGMEM char c_comma_str[] = "c,";
// ( char -- )
static void _c_comma(void) {
  *(char*)pHere++ = (char)pop();
}

const PROGMEM char c_fetch_str[] = "c@";
// ( c-addr -- char )
static void _c_fetch(void) {
  volatile uint8_t *address = (uint8_t*)pop();
  push(*address);
}

const PROGMEM char cell_plus_str[] = "cell+";
// ( a-addr1 -- a-addr2 )
static void _cell_plus(void) {
  push((addr_t)(pop()+ sizeof(cell_t)));
}

const PROGMEM char cells_str[] = "cells";
// ( n1 -- n2 )
// n2 is the size in address units of n1 cells.
static void _cells(void) {
  push(pop()*sizeof(cell_t));
}

const PROGMEM char char_str[] = "char";
// ( "<spaces>name" -- char )
// Skip leading space delimiters. Parse name delimited by a space. Put the value
// of its first character onto the stack.
static void _char(void) {
  if(getToken()) push(cTokenBuffer[0]);
  else {
    push(-16);
    _throw();
  }
}

const PROGMEM char char_plus_str[] = "char+";
// ( c-addr1 -- c-addr2 )
static void _char_plus(void) {
  push(pop() + 1);
}

const PROGMEM char chars_str[] = "chars";
// ( n1 -- n2 )
// n2 is the size in address units of n1 characters.
static void _chars(void) {
}

const PROGMEM char constant_str[] = "constant";
// ( x"<spaces>name" --  )
static void _constant(void) {
  openEntry();
  *(cell_t*)pHere = (cell_t)LITERAL_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  *(cell_t*)pHere = pop();
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  closeEntry();
}

const PROGMEM char count_str[] = "count";
// ( c-addr1 -- c-addr2 u )
static void _count(void) {
  char* addr = (char*)pop();
  push((cell_t)(addr+1));
  push(*addr);
}

const PROGMEM char cr_str[] = "cr";
// ( -- )
// Carriage Return
static void _cr(void) { 
  Serial.println();
}

const PROGMEM char create_str[] = "create";
// ( "<spaces>name" -- )
// Skip leading space delimiters. Parse name delimited by a space. Create a 
// definition for name with the execution semantics defined below. If the data-space
// pointer is not aligned, reserve enough data space to align it. The new data-space
// pointer defines name's data field. CREATE does not allocate data space in name's
// data field.
// name EXECUTION: ( -- a-addr )
// a-addr is the address of name's data field. The execution semantics of name may 
// be extended by using DOES>.
static void _create(void) {
  openEntry();
  *(cell_t*)pHere = LITERAL_IDX;
  pHere += sizeof(cell_t);
  // Location of Data Field at the end of the definition.
  *(cell_t*)pHere = (cell_t)pHere + 3 * sizeof(cell_t); 
  pHere += sizeof(cell_t);
  *(cell_t*)pHere = EXIT_IDX;   // Store an extra exit reference so 
                                // that it can be replace by a 
                                // subroutine pointer created by DOES>
  pDoes = (cell_t*)pHere;       // Save this location for uses by subroutine.
  pHere += sizeof(cell_t);
  if (!state) closeEntry();           // Close the entry if interpreting
}

const PROGMEM char decimal_str[] = "decimal";
// ( -- )
// Set BASE to 10
static void _decimal(void) { // value --
  base = 10;
}

const PROGMEM char depth_str[] = "depth";
// ( -- +n )
// +n is the number of single cells on the stack before +n was placed on it.
static void _depth(void) { // value --
  push(tos + 1);
}

const PROGMEM char do_str[] = "do";
// Compilation: (C: -- do-sys)
// Run-Time: ( n1|u1 n2|u2 -- ) (R: -- loop-sys )
static void _do(void) {
  push(DO_SYS);
  *(cell_t*)pHere = DO_SYS_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  push((cell_t)pHere); // store the origin address of the do loop 
}

const PROGMEM char does_str[] = "does>";
// Compilation: (C: colon-sys1 -- colon-sys2)
// Run-Time: ( -- ) (R: nest-sys1 -- )
// Initiation: ( i*x -- i*x a-addr ) (R: -- next-sys2 )
static void _does(void) {
  *(cell_t*)pHere = SUBROUTINE_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  // Store location for a subroutine call
  *(cell_t*)pHere = (cell_t)pHere + 2 * sizeof(cell_t);  
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  *(cell_t*)pHere = EXIT_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  // Start Subroutine coding
}

const PROGMEM char drop_str[] = "drop";
// ( x -- )
// Remove x from stack
static void _drop(void) {
    pop();
}

const PROGMEM char dupe_str[] = "dup";
// ( x -- x x )
// Duplicate x
static void _dupe(void) { 
    push(stack[tos]);
}

const PROGMEM char else_str[] = "else";
// Interpretation: Undefine
// Compilation: (C: orig1 -- orig2)
// Run-Time: ( -- )
static void _else(void) {
  cell_t* orig = (cell_t*)pop();
  *(cell_t*)pHere = JUMP_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  push((cell_t)pHere);
  pHere += sizeof(cell_t);
  *orig = (cell_t)pHere - (cell_t)orig;
#ifdef DEBUG
  debugValue(orig);
#endif
}

const PROGMEM char emit_str[] = "emit";
// ( x -- )
// display x as a character
static void _emit(void) {
  Serial.print((char) pop());
}

const PROGMEM char environment_str[] = "environment?";
// ( c-addr u  -- false|i*x true )
// c-addr is the address of a character string and u is the string's character
// count. u may have a value in the range from zero to an implementation-defined
// maximum which shall not be less than 31. The character string should contain
// a keyword from 3.2.6 Environmental queries or the optional word sets to to be
// checked for correspondence with  an attribute of the present environment.
// If the system treats the attribute as unknown, the return flag is false; 
// otherwise, the flag is true and i*x returned is the of the type specified in 
// the table for the attribute queried.
static void _environment(void) {
  char length = (char)pop();
  char* pStr = (char*)pop();
  if (length && length < STRING_SIZE) {
    if (!strcmp_P(pStr, PSTR("/counted-string"))) {
      push(STRING_SIZE);
      return;
    }
    if (!strcmp_P(pStr, PSTR("/hold"))) {
      push(HOLD_SIZE); 
      return;
    }
    if (!strcmp_P(pStr, PSTR("address-unit-bits"))) {
      push(ADDRESS_BITS); 
      return;
    }
    if (!strcmp_P(pStr, PSTR("core"))) {
      push(FALSE); 
      return;
    }
    if (!strcmp_P(pStr, PSTR("core-ext"))) {
      push(FALSE); 
      return;
    }
    if (!strcmp_P(pStr, PSTR("floored"))) {
      push(FLOORED); 
      return;
    }
    if (!strcmp_P(pStr, PSTR("max-char"))) {
      push(MAX_CHAR); 
      return;
    }
    if (!strcmp_P(pStr, PSTR("max-d"))) {
      push(MAX_D); 
      return;
    }
    if (!strcmp_P(pStr, PSTR("max-n"))) {
      push(MAX_N); 
      return;
    }
    if (!strcmp_P(pStr, PSTR("max-u"))) {
      push(MAX_U); 
      return;
    }
    if (!strcmp_P(pStr, PSTR("max-ud"))) {
      push(MAX_UD); 
      return;
    }
    if (!strcmp_P(pStr, PSTR("return-stack-size"))) {
      push(RSTACK_SIZE); 
      return;
    }
    if (!strcmp_P(pStr, PSTR("stack-size"))) {
      push(STACK_SIZE); 
      return;
    }
  }
  push(-13);
  _throw();
}

const PROGMEM char evaluate_str[] = "evaluate";
// ( i*x c-addr u  -- j*x )
// Save the current input source specification. Store minus-one (-1) in SOURCE-ID
// if it is present. Make the string described by c-addr and u both the input 
// source and input buffer, set >IN to zero, and interpret. When the parse area
// is empty, restore the prior source specification. Other stack effects are due
// to the words EVALUATEd.
static void _evaluate(void) {
  char* tempSource = cpSource;
  char* tempSourceEnd = cpSourceEnd;
  char* tempToIn = cpToIn;
  
  uint8_t length = pop();
  cpSource = (char*)pop();
  cpSourceEnd = cpSource + length;
  cpToIn = cpSource;
  interpreter();
  cpSource = tempSource;
  cpSourceEnd = tempSourceEnd;
  cpToIn = tempToIn;
}

const PROGMEM char execute_str[] = "execute";
// ( i*x xt -- j*x )
// Remove xt from the stack and preform the semantics identified by it. Other
// stack effects are due to the word EXECUTEd
static void _execute(void) {
  func function;
  w = pop();
  if (w > 255) {
    rPush(0);            // Push 0 as our return address
    ip = (cell_t *)w;          // set the ip to the XT (memory location)
    executeWord();
  } else {
    function = (func) pgm_read_word(&(flashDict[w - 1].function));
    function();
    if (errorCode) return;
  }
}

const PROGMEM char exit_str[] = "exit";
// Interpretation: undefined
// Execution: ( -- ) (R: nest-sys -- )
// Return control to the calling definition specified by nest-sys. Before 
// executing EXIT within a do-loop, a program shall discard the loop-control
// parameters by executing UNLOOP.
static void _exit(void) {
  ip = (cell_t*)rPop();
#ifdef DEBUG
  debugNewIP();
#endif
}

const PROGMEM char fill_str[] = "fill";
// ( c-addr u char -- )
// if u is greater than zero, store char in u consecutive characters of memory
// beginning with c-addr.
static void _fill(void) {
  char ch = (char)pop();
  cell_t limit = pop();
  char* addr = (char*)pop();
  for(int i = 1; i < limit; i++) {
    *addr++ = ch;
  }
}

const PROGMEM char find_str[] = "find";
// ( c-addr -- c-addr 0 | xt 1 | xt -1)
// Find the definition named in the counted string at c-addr. If the definition
// is not found, return c-addr and zero. If the definition is found, return its
// execution token xt. If the definition is immediate, also return one (1), 
// otherwise also return minus-one (-1).
static void _find(void) {
  uint8_t index = 0;

  char* ptr = (char*)pop();
  uint8_t length = *ptr++;
  if (length = 0) {
    push(-16);
    _throw();
    return;
  } else if (length > STRING_SIZE) {
    push(-18);
    _throw();
    return;
  }
   
  pUserEntry = pLastUserEntry;
  // First search through the user dictionary
  while(pUserEntry) {
    if (strcmp(pUserEntry->name, ptr) == 0) {
      length = strlen(pUserEntry->name);
 //     w = (cell_t)pUserEntry + length + 4;
 //     // Align the address in w
 //     ALIGN(w);
 //     push(w);
      push(pUserEntry->cfa);
      wordFlags = pUserEntry->flags;
//      if(pUserEntry->flags & IMMEDIATE) push(1);
      if(wordFlags & IMMEDIATE) push(1);
      else push(-1);
      return;
    }
    pUserEntry = (userEntry_t*)pUserEntry->prevEntry;
  }
  // Second Search through the flash Dictionary
  while(pgm_read_word(&(flashDict[index].name))) {
    if (!strcasecmp_P(ptr, (char*) pgm_read_word(&(flashDict[index].name)))) {
      push(index + 1);
      wordFlags = pgm_read_byte(&(flashDict[index].flags)); 
//      if(pgm_read_byte(&(flashDict[index].flags)) & IMMEDIATE) push(1);
      if(wordFlags & IMMEDIATE) push(1);
      else push(-1);
      return;                               
    }
    index++;
  }
  push((cell_t)ptr);
  push(0);
}

const PROGMEM char fm_slash_mod_str[] = "fm/mod";
// ( d1 n1 -- n2 n3 )
// Divide d1 by n1, giving the floored quotient n3 and remainder n2.
static void _fm_slash_mod(void) {
  cell_t n1 = pop();
  cell_t d1 = pop();
  push(d1 /  n1);  
  push(d1 %  n1);  
}

const PROGMEM char here_str[] = "here";
// ( -- addr )
// addr is the data-space pointer.
static void _here(void) {
  push((cell_t)pHere);
}

const PROGMEM char hold_str[] = "hold";
// ( char -- )
// add char to the beginning of the pictured numeric output string.
static void _hold(void) {
  if (flags & NUM_PROC) {
    *--pPNO = (char) pop();
  }
}

const PROGMEM char i_str[] = "i";
// Interpretation: undefined
// Execution: ( -- n|u ) (R: loop-sys -- loop-sys )
static void _i(void) {
  push(rStack[rtos - 1]); 
}

const PROGMEM char if_str[] = "if";
// Compilation: (C: -- orig )
// Run-Time: ( x -- )
static void _if(void) {
  *(cell_t*)pHere = ZJUMP_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  *(cell_t*)pHere = 0;
  push((cell_t)pHere);
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere +=sizeof(cell_t);
}

const PROGMEM char immediate_str[] = "immediate";
// ( -- )
// make the most recent definition an immediate word.
static void _immediate(void) {
  if (pLastUserEntry) {
    pLastUserEntry->flags |= IMMEDIATE;
  }
}

const PROGMEM char invert_str[] = "invert";
// ( x1 -- x2 )
// invert all bits in x1, giving its logical inverse x2
static void _invert(void)   { 
  push(~pop());
}

const PROGMEM char j_str[] = "j";
// Interpretation: undefined
// Execution: ( -- n|u ) (R: loop-sys1 loop-sys2 -- loop-sys1 loop-sys2 )
// n|u is a copy of the next-outer loop index. An ambiguous condition exists 
// if the loop control parameters of the next-outer loop, loop-sys1, are
// unavailable.
static void _j(void) {
  push(rStack[rtos - 4]); 
}

const PROGMEM char key_str[] = "key";
// ( -- char )
static void _key(void) {
  push(getKey());
}

const PROGMEM char leave_str[] = "leave";
// Interpretation: undefined
// Execution: ( -- ) (R: loop-sys -- )
static void _leave(void) {
  *(cell_t*)pHere = LEAVE_SYS_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere +=sizeof(cell_t);
  push((cell_t)pHere);
  *(cell_t*)pHere = 0;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere +=sizeof(cell_t);
  _swap();
}

const PROGMEM char literal_str[] = "literal";
// Interpretation: undefined
// Compilation: ( x -- )
// Run-Time: ( -- x )
// Place x on the stack
static void _literal(void) {
  if (state) {
    *(cell_t*)pHere = (cell_t)LITERAL_IDX;
#ifdef DEBUG
    debugXT((cell_t*)pHere);
#endif
    pHere += sizeof(cell_t);
    *(cell_t*)pHere = pop();
#ifdef DEBUG
    debugXT(pHere);
#endif
    pHere += sizeof(cell_t);
  } else {
    push(*ip++);
  } 
}

const PROGMEM char loop_str[] = "loop";
// Interpretation: undefined
// Compilation: (C: do-sys -- )
// Run-Time: ( -- ) (R: loop-sys1 -- loop-sys2 )
static void _loop(void) {
  *(cell_t*)pHere = LOOP_SYS_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  *(cell_t*)pHere = pop();
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  cell_t* leave = (cell_t*)pop();
  if (leave != (cell_t*)DO_SYS) {
    if (stack[tos] == DO_SYS) {
      *leave = (ucell_t)pHere;
      pop();
    } else {
      push(-22);
      _throw();
      return;
    }
  }
}

const PROGMEM char lshift_str[] = "lshift";
// ( x1 u -- x2 )
// x2 is x1 shifted to left by u positions.
static void _lshift(void) {
  cell_t u = pop();
  cell_t x1 = pop();
  push(x1 << u);
}

const PROGMEM char m_star_str[] = "m*";
// ( n1 n2 -- d )
// d is the signed product of n1 times n2.
static void _m_star(void) {
  push(pop() * pop());
}

const PROGMEM char max_str[] = "max";
// ( n1 n2 -- n3 )
// n3 is the greater of of n1 or n2.
static void _max(void) {
  cell_t n2 = pop();
  cell_t n1 = pop();
  if (n1 > n2) push(n1);
  else push(n2);
}

const PROGMEM char min_str[] = "min";
// ( n1 n2 -- n3 )
// n3 is the lesser of of n1 or n2.
static void _min(void) {
  cell_t n2 = pop();
  cell_t n1 = pop();
  if (n1 > n2) push(n2);
  else push(n1);
}

const PROGMEM char mod_str[] = "mod";
// ( n1 n2 -- n3 )
// Divide n1 by n2 giving the remainder n3.
static void _mod(void) {
  cell_t temp = pop();
  push(pop() %  temp);
}

const PROGMEM char move_str[] = "move";
// ( addr1 addr2 u -- )
// if u is greater than zero, copy the contents of u consecutive address 
// starting at addr1 to u consecutive address starting at addr2.
static void _move(void) {
  cell_t u = pop();
  addr_t *to = (addr_t*)pop();
  addr_t *from = (addr_t*)pop();
  for (cell_t i = 0; i < u; i++) {
    *to++ = *from++;
  }
}

const PROGMEM char negate_str[] = "negate";
// ( n1 -- n2 )
// Negate n1, giving its arithmetic inverse n2.
static void _negate(void) { 
  push(!pop());
}

const PROGMEM char or_str[] = "or";
// ( x1 x2 -- x3 )
// x3 is the bit by bit logical or of x1 with x2
static void _or(void) { 
  push(pop() |  pop());
}

const PROGMEM char over_str[] = "over";
// ( x y -- x y x )
static void _over(void) { 
    push(stack[tos - 1]);
}

const PROGMEM char postpone_str[] = "postpone";
// Compilation: ( "<spaces>name" -- )
// Skip leading space delimiters. Parse name delimited by a space. Find name. 
// Append the compilation semantics of name to the current definition. An 
// ambiguous condition exists if name is not found.
static void _postpone(void) { 
  func function;
  if(!getToken()) {
    push(-16);
    _throw();
  }
  if(isWord(cTokenBuffer)) {
    if(wordFlags & COMP_ONLY) {
      if (w > 255) {
        rPush(0);            // Push 0 as our return address
        ip = (cell_t *)w;          // set the ip to the XT (memory location)
        executeWord();
      } else {
        function = (func) pgm_read_word(&(flashDict[w - 1].function));
        function();
        if (errorCode) return;
      }
    } else {
      *(cell_t*)pHere = (cell_t)w;
#ifdef DEBUG
      debugXT(pHere);
#endif
      pHere += sizeof(cell_t);
    }
  } else {
    push(-13);
    _throw();
    return;
  }
}

const PROGMEM char quit_str[] = "quit";
// ( -- ) (R: i*x -- )
// Empty the return stack, store zero in SOURCE-ID if it is present,
// make the user input device the input source, enter interpretation state.
static void _quit(void) { 
  rtos = -1;
  *cpToIn = 0;          // Terminate buffer to stop interpreting
  Serial.flush();
}

const PROGMEM char r_from_str[] = "r>";
// Interpretation: undefined
// Execution: ( -- x ) (R: x -- )
// move x from the return stack to the data stack.
static void _r_from(void) {
#ifdef DEBUG
  ucell_t temp = rPop();
  serial_print_P(PSTR("  Moving $"));
  Serial.print(temp, HEX);
  serial_print_P(PSTR(" To Data Stack\r\n"));
  push(temp);
#else
  push(rPop());
#endif
}

const PROGMEM char r_fetch_str[] = "r@";
// Interpretation: undefined
// Execution: ( -- x ) (R: x -- x)
// Copy x from the return stack to the data stack.
static void _r_fetch(void) { 
  push(rStack[rtos]);
}

const PROGMEM char recurse_str[] = "recurse";
// Interpretation: Interpretation semantics for this word are undefined
// Compilation: ( -- ) 
// Append the execution semantics of the current definition to the current
// definition. An ambiguous condition exists if RECURSE appends in a definition 
// after DOES>.
static void _recurse(void) { 
  *(cell_t*)pHere = (cell_t)pCodeStart;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
}

const PROGMEM char repeat_str[] = "repeat";
// Interpretation: undefined
// Compilation: (C: orig dest -- )
// Run-Time ( -- )
// Continue execution at the location given.
static void _repeat(void) { 
  cell_t dest;
  cell_t* orig;
  *(cell_t*)pHere = JUMP_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  *(cell_t*)pHere = pop() - (cell_t)pHere;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  orig = (cell_t*)pop();
  *orig = (cell_t)pHere - (cell_t)orig;
#ifdef DEBUG
  debugValue(orig);
#endif
}

const PROGMEM char rot_str[] = "rot";
// ( x1 x2 x3 -- x2 x3 x1)
static void _rot(void) { 
  cell_t x3 = pop();
  cell_t x2 = pop();
  cell_t x1 = pop();
  push(x2);
  push(x3);
  push(x1);
}

const PROGMEM char rshift_str[] = "rshift";
// ( x1 u -- x2 )
// x2 is x1 shifted to right by u positions.
static void _rshift(void) {
  cell_t u = pop();
  cell_t x1 = pop();
  push(x1 >> u);
}

const PROGMEM char s_quote_str[] = "s\x22"; 
// Interpretation: Interpretation semantics for this word are undefined.
// Compilation: ("ccc<quote>" -- )
// Parse ccc delimited by ". Append the run-time semantics given below to the 
// current definition.
// Run-Time: ( -- c-addr u )
// Return c-addr and u describing a string consisting of the characters ccc. A program
// shall not alter the returned string.
static void _s_quote(void) {
  uint8_t i;
  char length;
  if (flags & EXECUTE) {
    push((cell_t)ip);
    cell_t len = strlen((char*)ip);
    push(len++);    // increment for the null terminator
    ALIGN(len);
    ip = (cell_t*)((cell_t)ip + len);
  }
  else if (state) {
    cDelimiter = '"';
    if(!getToken()) {
      push(-16);
      _throw();
    }
    length = strlen(cTokenBuffer);
    *(cell_t*)pHere = (cell_t)S_QUOTE_IDX;
  #ifdef DEBUG
    debugXT(pHere);
  #endif
    pHere += sizeof(cell_t);
#ifdef DEBUG
    serial_print_P(PSTR("\r\n  String @ $"));
    char* str = (char*)pHere;
    Serial.print((ucell_t)str, HEX);
#endif
    for (uint8_t i = 0; i < length; i++) {
      *(char*)pHere++ = cTokenBuffer[i];
    }
    *(char*)pHere++ = NULL;    // Terminate String
#ifdef DEBUG
    serial_print_P(PSTR(": "));
    Serial.print(str);
#endif
    ALIGN_P(pHere);  // re- align the pHere for any new code
    cDelimiter = ' ';
  }
}

const PROGMEM char s_to_d_str[] = "s>d";
// ( n -- d )
static void _s_to_d(void) {
  cell_t n = pop();
  push(0);
  push(n);
}

const PROGMEM char sign_str[] = "sign";
// ( n -- )
static void _sign(void) {
  if (flags & NUM_PROC) {
    cell_t sign = pop();
    if (sign < 0) *--pPNO = '-';
  }
}

const PROGMEM char sm_slash_rem_str[] = "sm/rem";
// ( d1 n1 -- n2 n3 )
// Divide d1 by n1, giving the symmetric quotient n3 and remainder n2.
static void _sm_slash_rem(void) {
  cell_t n1 = pop();
  cell_t d1 = pop();
  push(d1 /  n1);  
  push(d1 %  n1);  
}

const PROGMEM char source_str[] = "source";
// ( -- c-addr u )
// c-addr is the address of, and u is the number of characters in, the input buffer.
static void _source(void) {
  push((cell_t)&cInputBuffer);
  push(strlen(cInputBuffer));
}

const PROGMEM char space_str[] = "space";
// ( -- )
// Display one space
static void _space(void) {
  serial_print_P(sp_str);
}

const PROGMEM char spaces_str[] = "spaces";
// ( n -- )
// if n is greater than zero, display n space
static void _spaces(void) {
  char n = (char) pop();
  while (n > 0) {
    serial_print_P(sp_str);
  }
}

const PROGMEM char state_str[] = "state";
// ( -- a-addr )
// a-addr is the address of the cell containing compilation state flag.
static void _state(void) {
  push((cell_t)&state);
}

const PROGMEM char swap_str[] = "swap";
static void _swap(void) { // x y -- y x
  cell_t x, y;
  
  y = pop();
  x = pop();
  push(y);
  push(x);
}

const PROGMEM char then_str[] = "then";
// Interpretation: Undefine
// Compilation: (C: orig -- )
// Run-Time: ( -- )
static void _then(void) {
  cell_t* orig = (cell_t*)pop();
  *orig = (cell_t)pHere - (cell_t)orig;
#ifdef DEBUG
  debugValue(orig);
#endif
}

const PROGMEM char type_str[] = "type";
// ( c-addr u -- )
// if u is greater than zero display character string specified by c-addr and u
static void _type(void) {
  uint8_t length = (uint8_t)pop();
  char* addr = (char*)pop();
#ifdef DEBUG
  _cr();
#endif
  for (char i = 0; i < length; i++) 
    Serial.print(*addr++);
}

const PROGMEM char u_dot_str[] = "u.";
// ( u -- )
// Displau u in free field format
static void _u_dot(void) {
  Serial.print((ucell_t) pop());
}

const PROGMEM char u_lt_str[] = "u<";
// ( u1 u2 -- flag )
// flag is true if and only if u1 is less than u2.
static void _u_lt(void) {
  if ((ucell_t)pop() > ucell_t(pop())) push(TRUE);
  else push(FALSE);
}

const PROGMEM char um_star_str[] = "um*";
// ( u1 u2 -- ud )
// multiply u1 by u2, giving the unsigned double-cell product ud
static void _um_star(void) {
  udcell_t ud = pop() * pop();
  cell_t lsb = (ucell_t)ud;
  cell_t msb = (ucell_t)(ud >> sizeof(ucell_t)*8);
  push(msb);
  push(lsb);
}

const PROGMEM char um_slash_mod_str[] = "um/mod";
// ( ud u1 -- u2 u3 )
// Divide ud by u1 giving quotient u3 and remainder u2.
static void _um_slash_mod(void) {
  ucell_t u1 = pop();
  udcell_t lsb = pop();
  udcell_t msb = pop();
  udcell_t ud = (msb << 16) + (lsb);
  push(ud % u1);
  push(ud / u1);
}

const PROGMEM char unloop_str[] = "unloop";
// Interpretation: Undefine
// Execution: ( -- )(R: loop-sys -- )
static void _unloop(void) {
  serial_print_P(not_done_str); 
  rPop();
  rPop();
  if (rPop() != LOOP_SYS) {
    push(-22);
    _throw();
  }
}

const PROGMEM char until_str[] = "until";
// Interpretation: Undefine
// Compilation: (C: dest -- )
// Run-Time: ( x -- )
static void _until(void) {
  *(cell_t*)pHere = ZJUMP_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  *(cell_t*)pHere = pop() - (cell_t)pHere;
#ifdef DEBUG
  debugValue(pHere);
#endif
  pHere += sizeof(cell_t);
}

const PROGMEM char variable_str[] = "variable";
// ( "<spaces>name" -- )
// Parse name delimited by a space. Create a definition for name with the
// execution semantics defined below. Reserve one cell of data space at an 
// aligned address.
// name Execution: ( -- a-addr )
// a-addr is the address of the reserved cell. A program is responsible for
// initializing the contents of a reserved cell.
static void _variable(void) {
  if (flags & EXECUTE) {
    push((cell_t)ip++);    
  } else {
    openEntry();
    *(cell_t*)pHere = (cell_t)VARIABLE_IDX;
#ifdef DEBUG
    debugXT(pHere);
#endif
    pHere += sizeof(cell_t);
    *(cell_t*)pHere = 0;
#ifdef DEBUG
    debugXT(pHere);
#endif
    pHere += sizeof(cell_t);
    closeEntry();
  }
}

const PROGMEM char while_str[] = "while";
// Interpretation: Undefine
// Compilation: (C: dest -- orig dest )
// Run-Time: ( x -- )
static void _while(void) {
  ucell_t dest;
  ucell_t orig;
  dest = pop();
  *(cell_t*)pHere = ZJUMP_IDX;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  orig = (cell_t)pHere;
  *(cell_t*)pHere = 0;
#ifdef DEBUG
  debugXT(pHere);
#endif
  pHere += sizeof(cell_t);
  push(orig);
  push(dest);
}

const PROGMEM char word_str[] = "word";
// ( char "<chars>ccc<chars>" -- c-addr )
// Skip leading delimiters. Parse characters ccc delimited by char. An ambiguous
// condition exists if the length of the parsed string is greater than the 
// implementation-defined length of a counted string.
//
// c-addr is the address of a transient region containing the parsed word as a
// counted string. If the parse area was empty or contained no characters other than
// the delimiter, the resulting string has a zero length. A space, not included in 
// the length, follows the string. A program may replace characters within the 
// string.
//
// NOTE: The requirement to follow the string with a space is obsolescent and is
// included as a concession to existing programs that use CONVERT. A program shall 
// not depend on the existence of the space.
static void _word(void) {
  uint8_t* start;
//  char* cPtr = &cTokenBuffer[1];
  cDelimiter = (char)pop();
  start = pHere++;
  while(cpToIn <= cpSourceEnd) {
    if (*cpToIn == cDelimiter || *cpToIn == 0) {
      *start = (pHere - start) - 1;       // write the length byte
//      cTokenBuffer[0] = (cPtr - cTokenBuffer) - 1;       // write the length byte
      pHere = start;                      // reset pHere (transient memory)
      push((cell_t)start);                // push the c-addr onto the stack
//      push((cell_t)cPtr);                   // push the c-addr onto the stack
      cpToIn++;
      break;      
    } else *pHere++ = *cpToIn++;
//    } else *cPtr++ = *cpToIn++;
  }
  cDelimiter = ' ';
}

const PROGMEM char xor_str[] = "xor";
// ( x1 x2 -- x3 )
// x3 is the bit by bit exclusive or of x1 with x2
static void _xor(void) { 
  push(pop() ^  pop());
}

const PROGMEM char left_bracket_str[] = "[";
// Interpretation: undefined
// Compilation: Preform the execution semantics given below
// Execution: ( -- )
// Enter interpretation state. [ is an immediate word.
static void _left_bracket(void) {
  state = FALSE;
}

const PROGMEM char bracket_tick_str[] = "[']";
// Interpretation: Interpretation semantics for this word are undefined.
// Compilation: ( "<space>name" -- )
// Skip leading space delimiters. Parse name delimited by a space. Find name. 
// Append the run-time semantics given below to the current definition.
// An ambiguous condition exist if name is not found.
// Run-Time: ( -- xt )
// Place name's execution token xt on the stack. The execution token returned 
// by the compiled phrase "['] X" is the same value returned by "' X" outside
// of compilation state.
static void _bracket_tick(void) {
  if(!getToken()) {
    push(-16);
    _throw();
  }
  if(isWord(cTokenBuffer)) {
    *(cell_t*)pHere = LITERAL_IDX;
    pHere += sizeof(cell_t);
    *(cell_t*)pHere = w;
    pHere += sizeof(cell_t);
  } else {
    push(-13);
    _throw();
    return;
  }
}

const PROGMEM char bracket_char_str[] = "[char]";
// Interpretation: Interpretation semantics for this word are undefined.
// Compilation: ( "<space>name" -- )
// Skip leading spaces delimiters. Parse name delimited by a space. Append
// the run-time semantics given below to the current definition.
// Run-Time: ( -- char )
// Place char, the value of the first character of name, on the stack.
static void _bracket_char(void) {
  if(getToken()) {
    *(cell_t*)pHere = LITERAL_IDX;
    pHere += sizeof(cell_t);
    *(cell_t*)pHere = cTokenBuffer[0];
    pHere += sizeof(cell_t);
  } else {
    push(-16);
    _throw();
  }
}

const PROGMEM char right_bracket_str[] = "]";
// ( -- )
// Enter compilation state.
static void _right_bracket(void) {
  state = TRUE;
}

/*******************************************************************************/
/**                          Core Extension Set                               **/
/*******************************************************************************/
#ifdef CORE_EXT_SET
const PROGMEM char neq_str[] = "<>";
static void _neq(void) { 
  push(pop() != pop()); 
}
const PROGMEM char hex_str[] = "hex";
// ( -- )
// Set BASE to 16
static void _hex(void) { // value --
  base = 16;
}
#endif

/*******************************************************************************/
/**                            Double Cell Set                                **/
/*******************************************************************************/
#ifdef DOUBLE_SET
#endif

/*******************************************************************************/
/**                             Exception Set                                 **/
/*******************************************************************************/
#ifdef EXCEPTION_SET
const PROGMEM char throw_str[] = "throw";
// ( k*x n -- k*x | i*x n)
// if any bit of n are non-zero, pop the topmost exception frame from the 
// exception stack, along with everything on the return stack above that frame.
// ...
static void _throw(void) {
  errorCode = pop();
  uint8_t index = 0;
  int8_t tableCode;
  _cr();
  Serial.print(cTokenBuffer);
  serial_print_P(PSTR(" EXCEPTION("));
  do{
    tableCode = pgm_read_byte(&(exception[index].code));
    if (errorCode == tableCode) {
      Serial.print((int)errorCode);
      serial_print_P(PSTR("): "));
      serial_print_P((char*) pgm_read_word(&exception[index].name));
      _cr();
    }
    index++;
  } while(tableCode);
  tos = -1;                       // Clear the stack.
  _quit();
  state = FALSE;
}  
#endif

/*******************************************************************************/
/**                              Local Set                                    **/
/*******************************************************************************/
#ifdef LOCAL_SET
#endif

/*******************************************************************************/
/**                              Memory Set                                   **/
/*******************************************************************************/
#ifdef MEMORY_SET
#endif

/*******************************************************************************/
/**                          Programming Tools Set                            **/
/*******************************************************************************/
#ifdef TOOLS_SET
const PROGMEM char dot_s_str[] = ".s";
static void _dot_s(void) {
  char i;
  char depth = tos + 1;
  if (tos >= 0) {
    for (i = 0; i < depth ; i++) {
      w = stack[i];
      displayValue();
    }
  }
}

const PROGMEM char dump_str[] = "dump";
// ( addr u -- )
// Display the contents of u consecutive address starting at addr. The format of 
// the display is implementation dependent. 
// DUMP may be implemented using pictured numeric output words. Consequently, 
// its use may corrupt the transient region identified by #>.
static void _dump(void) { 
  uint8_t len = (uint8_t)pop();
  addr_t addr_start = (addr_t)pop();
  addr_t addr_end = addr_start;
  addr_end += len;
  addr_start = addr_start & 0xFFF0;
  
  volatile uint8_t* addr = (uint8_t*)addr_start;
  
  while (addr < (uint8_t*)addr_end) {
    serial_print_P(PSTR("\r\n$"));
    if (addr < (uint8_t*)0x10) serial_print_P(zero_str);
    if (addr < (uint8_t*)0x100) serial_print_P(zero_str);
    Serial.print((uint16_t)addr, HEX);
    serial_print_P(sp_str);
    for (uint8_t i = 0; i < 16; i++) {
      if (*addr < 0x10) serial_print_P(zero_str);
      Serial.print(*addr++, HEX);
      serial_print_P(sp_str);
    }
    serial_print_P(tab_str);
    addr -= 16;
    for (uint8_t i = 0; i < 16; i++) {
      if (*addr < 127 && *addr > 31) 
      Serial.print((char)*addr);
      else serial_print_P(PSTR("."));
      addr++;  
    }
  }
}

const PROGMEM char see_str[] = "see";
// ("<spaces>name" -- )
// Display a human-readable representation of the named word's definition. The
// source of the representation (object-code decompilation, source block, etc.)
// and the particular form of the display in implementation defined.
static void _see(void) { 
  _tick();
  char flags = wordFlags;
  if (flags && IMMEDIATE) 
    serial_print_P(PSTR("\r\nImmediate Word"));
  cell_t xt = pop();
  if (xt < 255) {
    serial_print_P(PSTR("\r\nWord is a primitive"));
  } else {
    cell_t* addr = (cell_t*)xt;
    serial_print_P(PSTR("\r\nCode Field Address: "));
    Serial.print((addr_t)addr);
    serial_print_P(PSTR("\r\nAddr\tXT\tName"));
    do {
      serial_print_P(PSTR("\r\n$"));
      Serial.print((cell_t)addr, HEX);
      serial_print_P(tab_str);
      Serial.print(*addr);
      serial_print_P(tab_str);
      xtToName(*addr);
      switch (*addr) {
        case 2:
        case 4:
        case 5:
          serial_print_P(PSTR("("));
          Serial.print(*++addr);
          serial_print_P(PSTR(")"));
          break;
        case 13:
        case 14:
          serial_print_P(sp_str);
          char *ptr = (char*)++addr;
          do {
            Serial.print(*ptr++);
          } while (*ptr != 0);
          serial_print_P(PSTR("\x22"));
          addr = (cell_t*)++ptr;
          addr = (cell_t*)(((cell_t)addr - 1) & -2);
          break;
      }
    } while (*addr++ != 1);
  }  
}

const PROGMEM char words_str[] = "words";
static void _words(void) { // --
  uint8_t count = 0;
  uint8_t index = 0;
  uint8_t length = 0;
  char* pChar;
  
  while (pgm_read_word(&(flashDict[index].name))) {
      if (count > 70) {
          Serial.println();
          count = 0;
      }
      if (!(pgm_read_word(&(flashDict[index].flags)) & SMUDGE)) {
        count += serial_print_P((char*) pgm_read_word(&(flashDict[index].name)));
//        count += serial_print_P(PSTR(" "));
        count += serial_print_P(sp_str);
      }
      index++;
  }
  
  pUserEntry = pLastUserEntry;
  while(pUserEntry) {
    if (count > 70) {
        Serial.println();
        count = 0;
    }
    if (!(pUserEntry->flags & SMUDGE)) {
      count += Serial.print(pUserEntry->name);
//      count += serial_print_P(PSTR(" "));
      count += serial_print_P(sp_str);
    }
    pUserEntry = (userEntry_t*)pUserEntry->prevEntry;
  }
  Serial.println();
}

#endif

/*******************************************************************************/
/**                               Search Set                                  **/
/*******************************************************************************/
#ifdef SEARCH_SET
#endif

/*******************************************************************************/
/**                               String Set                                  **/
/*******************************************************************************/
#ifdef STRING_SET
#endif

/********************************************************************************/
/**                         EEPROM Operations                                  **/
/********************************************************************************/
#ifdef EN_EEPROM_OPS
const PROGMEM char eeRead_str[] = "eeRead";
static void _eeprom_read(void) {             // address -- value
  push(EEPROM.read(pop()));
}

const PROGMEM char eeWrite_str[] = "eeWrite";
static void _eeprom_write(void) {             // value address -- 
  char address;
  char value;
  address = (char) pop();
  value = (char) pop();
  EEPROM.write(address, value);
}
#endif

/********************************************************************************/
/**                      Arduino Library Operations                            **/
/********************************************************************************/
#ifdef EN_ARDUINO_OPS
const PROGMEM char freeMem_str[] = "freeMem";
static void _freeMem(void) { 
  push(freeMem());
}

const PROGMEM char delay_str[] = "delay";
static void _delay(void) {
  delay(pop());
}

const PROGMEM char pinWrite_str[] = "pinWrite";
// ( u1 u2 -- )
// Write a high (1) or low (0) value to a digital pin 
// u1 is the pin and u2 is the value ( 1 or 0 ). To turn the LED attached to 
// pin 13 on type "1 13 pinwrite" p.s. First change its pinMode to output
static void _pinWrite(void) {
  digitalWrite(pop(),pop());
}

const PROGMEM char pinMode_str[] = "pinMode";
// ( u1 u2 -- )
// Set the specified pin behavior to either an input (0) or output (1)
// u1 is the pin and u2 is the mode ( 1 or 0 ). To control the LED attached to
// pin 13 to an output type "1 13 pinmode"
static void _pinMode(void) {
  pinMode(pop(), pop());
}

const PROGMEM char pinRead_str[] = "pinRead";
static void _pinRead(void) {
  push(digitalRead(pop()));
}

const PROGMEM char analogRead_str[] = "analogRead";
static void _analogRead(void) {
  push(analogRead(pop()));
}

const PROGMEM char analogWrite_str[] = "analogWrite";
static void _analogWrite(void) {
  analogWrite(pop(), pop());
}

const PROGMEM char to_name_str[] = ">name";
static void _toName(void) {
  xtToName(pop());
}
#endif

/*********************************************************************************/
/**                         Dictionary Initialization                           **/
/*********************************************************************************/
const PROGMEM flashEntry_t flashDict[] = {
    /*****************************************************/
    /* The initial entries must stay in this order so    */
    /* they always have the same index. They get called  */
    /* referenced when compiling code                    */
    /*****************************************************/
    { exit_str,           _exit,            NORMAL },
    { literal_str,        _literal,         IMMEDIATE },
    { type_str,           _type,            NORMAL },
    { jump_str,           _jump,            SMUDGE },
    { zjump_str,          _zjump,           SMUDGE },
    { subroutine_str,     _subroutine,      SMUDGE },
    { throw_str,          _throw,           NORMAL },
    { do_sys_str,         _do_sys,          SMUDGE },
    { loop_sys_str,       _loop_sys,        SMUDGE },
    { leave_sys_str,      _leave_sys,       SMUDGE },
    { plus_loop_sys_str,  _plus_loop_sys,   SMUDGE },
    { evaluate_str,       _evaluate,        NORMAL },
    { s_quote_str,        _s_quote,         IMMEDIATE + COMP_ONLY },
    { dot_quote_str,      _dot_quote,       IMMEDIATE + COMP_ONLY },
    { variable_str,       _variable,        NORMAL },
    
    /*****************************************************/
    /* Order does not matter after here                  */
    /*****************************************************/    
    { abort_str,          _abort,           NORMAL },
    { store_str,          _store,           NORMAL },
    { number_sign_str,    _number_sign,     NORMAL },
    { number_sign_gt_str, _number_sign_gt,  NORMAL },
    { number_sign_s_str,  _number_sign_s,   NORMAL },
    { tick_str,           _tick,            NORMAL },
    { paren_str,          _paren,           IMMEDIATE },
    { star_str,           _star,            NORMAL },
    { star_slash_str,     _star_slash,      NORMAL },
    { star_slash_mod_str, _star_slash_mod,  NORMAL },
    { plus_str,           _plus,            NORMAL },
    { plus_store_str,     _plus_store,      NORMAL },
    { plus_loop_str,      _plus_loop,       IMMEDIATE + COMP_ONLY },
    { comma_str,          _comma,           NORMAL },
    { minus_str,          _minus,           NORMAL },
    { dot_str,            _dot,             NORMAL },
    { slash_str,          _slash,           NORMAL },
    { slash_mod_str,      _slash_mod,       NORMAL },
    { zero_less_str,      _zero_less,       NORMAL },
    { zero_equal_str,     _zero_equal,      NORMAL },
    { one_plus_str,       _one_plus,        NORMAL },
    { one_minus_str,      _one_minus,       NORMAL },
    { two_store_str,      _two_store,       NORMAL },
    { two_star_str,       _two_star,        NORMAL },
    { two_slash_str,      _two_slash,       NORMAL },
    { two_fetch_str,      _two_fetch,       NORMAL },
    { two_drop_str,       _two_drop,        NORMAL },
    { two_dup_str,        _two_dup,         NORMAL },
    { two_over_str,       _two_over,        NORMAL },
    { two_swap_str,       _two_swap,        NORMAL },
    { colon_str,          _colon,           NORMAL },
    { semicolon_str,      _semicolon,       IMMEDIATE }, 
    { lt_str,             _lt,              NORMAL },
    { lt_number_sign_str, _lt_number_sign,  NORMAL },
    { eq_str,             _eq,              NORMAL },
    { gt_str,             _gt,              NORMAL },
    { to_body_str,        _to_body,         NORMAL },
    { to_in_str,          _to_in,           NORMAL },
    { to_number_str,      _to_number,       NORMAL },
    { to_r_str,           _to_r,            NORMAL },
    { question_dup_str,   _question_dup,    NORMAL },
    { fetch_str,          _fetch,           NORMAL },
    { abort_quote_str,    _abort_quote,     IMMEDIATE + COMP_ONLY },
    { abs_str,            _abs,             NORMAL },
    { accept_str,         _accept,          NORMAL },
    { align_str,          _align,           NORMAL },
    { aligned_str,        _aligned,         NORMAL },
    { allot_str,          _allot,           NORMAL },
    { and_str,            _and,             NORMAL },
    { base_str,           _base,            NORMAL },
    { begin_str,          _begin,           IMMEDIATE + COMP_ONLY },
    { bl_str,             _bl,              NORMAL },
    { c_store_str,        _c_store,         NORMAL },
    { c_comma_str,        _c_comma,         NORMAL },
    { c_fetch_str,        _c_fetch,         NORMAL },
    { cell_plus_str,      _cell_plus,       NORMAL },
    { cells_str,          _cells,           NORMAL },
    { char_str,           _char,            NORMAL },
    { char_plus_str,      _char_plus,       NORMAL },
    { chars_str,          _chars,           NORMAL },
    { constant_str,       _constant,        NORMAL },
    { count_str,          _count,           NORMAL },
    { cr_str,             _cr,              NORMAL },
    { create_str,         _create,          NORMAL },
    { decimal_str,        _decimal,         NORMAL },
    { depth_str,          _depth,           NORMAL },
    { do_str,             _do,              IMMEDIATE + COMP_ONLY },
    { does_str,           _does,            IMMEDIATE + COMP_ONLY },
    { drop_str,           _drop,            NORMAL },
    { dupe_str,           _dupe,            NORMAL },
    { else_str,           _else,            IMMEDIATE + COMP_ONLY },
    { emit_str,           _emit,            NORMAL },
    { environment_str,    _environment,     NORMAL },
    { execute_str,        _execute,         NORMAL },
    { fill_str,           _fill,            NORMAL },
    { find_str,           _find,            NORMAL },
    { fm_slash_mod_str,   _fm_slash_mod,    NORMAL },
    { here_str,           _here,            NORMAL },
    { hold_str,           _hold,            NORMAL },
    { i_str,              _i,               NORMAL },
    { if_str,             _if,              IMMEDIATE + COMP_ONLY },
    { immediate_str,      _immediate,       NORMAL },
    { invert_str,         _invert,          NORMAL },
    { j_str,              _j,               NORMAL },
    { key_str,            _key,             NORMAL },
    { leave_str,          _leave,           IMMEDIATE + COMP_ONLY },
    { loop_str,           _loop,            IMMEDIATE + COMP_ONLY },
    { lshift_str,         _lshift,          NORMAL },
    { m_star_str,         _m_star,          NORMAL },
    { max_str,            _max,             NORMAL },
    { min_str,            _min,             NORMAL },
    { mod_str,            _mod,             NORMAL },
    { move_str,           _move,            NORMAL },
    { negate_str,         _negate,          NORMAL },
    { or_str,             _or,              NORMAL },
    { over_str,           _over,            NORMAL },
    { postpone_str,       _postpone,        IMMEDIATE + COMP_ONLY },
    { quit_str,           _quit,            NORMAL },
    { r_from_str,         _r_from,          NORMAL },
    { r_fetch_str,        _r_fetch,         NORMAL },
    { recurse_str,        _recurse,         IMMEDIATE + COMP_ONLY },
    { repeat_str,         _repeat,          IMMEDIATE + COMP_ONLY },
    { rot_str,            _rot,             NORMAL },
    { rshift_str,         _rshift,          NORMAL },
    { s_to_d_str,         _s_to_d,          NORMAL },
    { sign_str,           _sign,            NORMAL },
    { sm_slash_rem_str,   _sm_slash_rem,    NORMAL },
    { source_str,         _source,          NORMAL },
    { space_str,          _space,           NORMAL },
    { spaces_str,         _spaces,          NORMAL },
    { state_str,          _state,           NORMAL },
    { swap_str,           _swap,            NORMAL },
    { then_str,           _then,            IMMEDIATE + COMP_ONLY },
    { u_dot_str,          _u_dot,           NORMAL },
    { u_lt_str,           _u_lt,            NORMAL },
    { um_star_str,        _um_star,         NORMAL },
    { um_slash_mod_str,   _um_slash_mod,    NORMAL },
    { unloop_str,         _unloop,          NORMAL + COMP_ONLY },
    { until_str,          _until,           IMMEDIATE + COMP_ONLY },
    { while_str,          _while,           IMMEDIATE + COMP_ONLY },
    { word_str,           _word,            NORMAL },
    { xor_str,            _xor,             NORMAL },
    { left_bracket_str,   _left_bracket,    IMMEDIATE },
    { bracket_tick_str,   _bracket_tick,    IMMEDIATE },
    { bracket_char_str,   _bracket_char,    IMMEDIATE },
    { right_bracket_str,  _right_bracket,   NORMAL },

#ifdef CORE_EXT_SET
    { neq_str,            _neq,             NORMAL },
    { hex_str,            _hex,             NORMAL },
#endif

#ifdef DOUBLE_SET
#endif

#ifdef EXCEPTION_SET
#endif

#ifdef LOCALS_SET
#endif

#ifdef MEMORY_SET
#endif

#ifdef TOOLS_SET
    { dot_s_str,          _dot_s,           NORMAL },
    { dump_str,           _dump,            NORMAL },
    { see_str,            _see,             NORMAL },
    { words_str,          _words,           NORMAL },
#endif

#ifdef SEARCH_SET
#endif

#ifdef STRING_SET
#endif
    
#ifdef EN_ARDUINO_OPS
    { freeMem_str,        _freeMem,         NORMAL },
    { delay_str,          _delay,           NORMAL },
    { pinWrite_str,       _pinWrite,        NORMAL },
    { pinMode_str,        _pinMode,         NORMAL },
    { pinRead_str,        _pinRead,         NORMAL },
    { analogRead_str,     _analogRead,      NORMAL },
    { analogWrite_str,    _analogWrite,     NORMAL },
    { to_name_str,        _toName,          NORMAL },
#endif

#ifdef EN_EEPROM_OPS
    { eeRead_str,     _eeprom_read,    NORMAL },
    { eeWrite_str,    _eeprom_write,    NORMAL },
#endif

    { NULL,           NULL,    NORMAL }
};


