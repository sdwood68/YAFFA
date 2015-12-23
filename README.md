YAFFA
=====

YAFFA - Yet Another Forth For Arduino

# Introduction:
YAFFA is a FORTH environment that runs on the Arduino Uno. It currently has a "working" interpreter and Compiler. "Working" because I've not been able to fully test all of the implemented words, and there may be other underlying peculiarities in the implementation. The eventual goal for the 1.0 release is to have a fully tested system, that is as compliant to the draft ANSI Forth Specification as described in DPANS94.

## Reason for writing yet another Forth for Arduino:
I am writing YAFFA in an attempt to learn more about FORTH. After reviewing a number for FORTH like programs for the Arduino and Atmel's ATMega micro-controllers I decided to write my own for a few reasons. First I wanted it written in C so that it would be portable, and more accessible to others. Second, the ones written in C, for Arduino, were very limited in the number of words supported.

##Releases:
###0.6.1
- Documentation cleanup. thanks to Dr. Hugh Sasse, BSc(Hons), PhD

###0.6
- Fixed PROGMEM compilation errors do to new compiler in Arduino 1.6
- Embedded the revision in to the compiled code.
- Revision is now displayed in greeting at start up.
- the interpreter not clears the word flags before it starts.
- Updated TICK, WORD, and FIND to make use of primitive calls to reduce code size.
- Added word flag checks in dot_quote() and _s_quote().

###0.5 Initial Release:
The initial release supports all core words in the draft standard except ">NUMBER". Though I don't guarantee I've implemented them all correctly ;-) Arduino specific words included are "pinMode", "pinRead", "pinWrite", "analogRead", "analogWrite", "eeRead", and "eeWrite"

##Frequently Asked Questions
###How do I get started using YAFFA?
- Download and extract the project into your sketchbook directory. If you don't know where that is open look under    "File->Preferences-> Sketchbook location". 
- The directory containing the code must be called  "Yaffa". 
- Once the directory is set up re-open the Arduino IDE, and look under "File->Scketchbook" for the YAFFA or Yaffa.
- Yaffa only work on the Uno or Leonard boards.
- In latest IDE (1.6.6) the compiler is giving errors for a number of functions declared static. you should be able to remove the word static from those functions and it should compile.
- The compiler will produce warnings "Low memory available, stability problems may occur." This is expected since the almost all the memory is reserved for the Forth environment. What is left over is used for the C stack. depending on how recursive your Forth programs are you could run out of stack space.
 
###Is this a full interactive Forth, or some sort of cross compiler?
This is an interactive Forth environment.

###Isn't implementing Forth in a HLL, like C++, counter-intuitive, or slow?
While implementing Forth in a HLL is slower than assembly, it greatly improves portability, which was one of my goals, and I wanted something the was native to the Arduino IDE. As I learn more about how Forth is suppose to work, I intend to try and improve performance were ever I can. 

###Is there real error-trapping capabilities?
There is limited error trapping, and what is implemented is bases on what I've gleaned from the draft specification. As I learn more, I will try and improve it. Detected errors cause an end to execution, compilation, or interpretation and the stacks are purged.

###Is there non-volatile storage of any kind for new words?  
Currently there is no non-volatile storage. The Arduino boot loader does not allow user applications to use the built in flash routines, so until that changes there won't be any flash storage. Eventually I want to be able to save and recall words into the internal EEPROM.

###Will some sort of filesystem be supported?
Maybe someday if I get an SDCARD shield, and the library's are not too big.

###Where are words are stored and if it is in RAM, how much space is available?
New words are stored in RAM. Since the ATmega328 only has 2K bytes of RAM, space is very limited, currently only 1K bytes are allocated to "Forth Space". I will try to improve this but it can't get much bigger since you need buffers for the serial input, stacks, global variables and the C stack.

###What Arduino specific words have been implemented?
I've implemented wrappers for pinRead, pinWrite, pinMode, eeRead, eeWrite, analogRead, ananlogWrite. As I start to write more program in Forth, I'm sure this will be expanded.

###Are you going to write any documentation?
Yes, as time permits. I will try and cover any specific questions people have as they come up.
