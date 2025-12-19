# EELE 367 CPU Assembler

## Purpose
EELE 367 at MSU requires students to "load" machine code instructions into the ROM entity in VHDL for their simulated 8 bit CPU. The instructions are represented by constants in the program, so there is already no need for the raw numerical values to be entered:

```vhdl
constant ROM : rom_type := (

0 => LDA_IMM,
1 => x"04",
2 => LDB_IMM,
3 => x"04",
4 => ADD_AB,
5 => BRA,
6 => x"00",	
others =>  x"00");
```

However, the formatting for these lines is quite annoying to deal with, especially for longer programs. Each time you move a line, you have to update the memory location numbers next to each instruction. Plus, it is very easy to miss an "x", forget to put quotes around the values, etc.

I found myself typing my programs in an "assembly-style":

```
LDA_IMM 04
LDB_IMM 04
ADD_AB
BRA 00
```
and then loading that program into the ROM by hand afterward. 

So the goal for this program is to create what I wish I had while taking this class: an "assembler" that will convert the readable assembly-style code written above into the formatted ROM entries needed for the VHDL program to run.

## Assembly File Formatting
This is the proper formatting required for your assembly program:
```
name= <name>
type= <type>

*CODE HERE*

HALT
```

* Replace `<name>` with the name of your ROM. In the above example, this would correspond to `ROM` following the `constant` keyword. 
* Replace `<type>` with the type of your ROM. In the above example, this would correspond to `rom_type`.
* In the `*CODE HERE*` section, enter the instructions for your program. Make sure your program will fit into the size of your ROM. The assembler caps the output to 128 memory slots, but doesn't ensure there is a final branch instruction: PROGRAMMER BEWARE.
* The `HALT` keyword is required at the end of your program.

## File I/O
* The input assembly file must be a `.txt` file.
* When prompted to enter the file name, enter an unquoted file path relative to the assembler that is less than 128 characters long such as `C:\program\files\asm.txt`
* The resulting output file will be sent to `ROM_load.txt`. You can then copy and paste the contents of this file directly into the ROM entity in your VHDL program.

## Example
This assembly code:
```
name= ROM
type= rom_type
LDA_IMM 04
LDB_IMM 04
ADD_AB
BRA 00
HALT
```
is assembled into this text in ROM_load.txt:

```vhdl
constant ROM : rom_type := (

0 => LDA_IMM,
1 => x"04",
2 => LDB_IMM,
3 => x"04",
4 => ADD_AB,
5 => BRA,
6 => x"00",	
others =>  x"00");
```

## NOTES AND RESTRICTIONS
* `ROM_SIZE` in `assembler.h` is set to 128 per the EELE 367 CPU's memory map. The assembler will output a maximum of this many ROM entries. Keep in mind that assembly lines that have an instruction and an operand will occupy 2 memory slots.
* There are no comments allowed. Your file must only contain the above contents. However, the assembler will catch extraneous text. This is a tool whipped up in a weekend, not an IDE :)
* All numbers and addresses are to be entered as two-digit hex values; essentially, there is no label resolution. EELE 371 and CSCI 366 give you that luxury. I chose to save myself the slight headache.
* This assembler assumes the use of the constants defined in the ROM architecture. It will not translate the raw machine code instructions.
* No, not every single user-possible error has been thought of, but most should get caught one way or another. If there are any horribly stupid errors found in this repo, please let me know.


