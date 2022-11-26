# C-Assembler
This assembler represents an imaginary partial assembly language that runs on an imaginary computer.\
Based on two pass assembler model.

### Usage
Written and compiled in Visual Studio 2019 on windows 10.\
Modified on Ubuntu 16.04 32-bit with gcc -Wall -ansi -pedantic to check for warnings.\
This Assembler can run on all ubuntu versions.
- Run makefile to compile the project from terminal:
```console
$ make
```
- Run Assembler on assembly files (without .as extension):
```console
$ ./Assembler Tests/ps
$ ./Assembler Tests/files_ps
```
The Assembler will generate the output files with the following extensions:
- **.ob** - object file, includes the machine code.
- **.ext** - externals file, includes details on all the addresses with external symbol encoded in data-words.
- **.ent** - entries file, includes details on every symbol defined as an entry point.

### Hardware
The imaginary computer setup includes CPU, registers and RAM.
- Total of 8 registers: r0, r1, r2, r3, r4, r5, r6, r7, r8. each register is the size of 12 bits.
- RAM size is 4096 cells, each cell is the size of 12 bits. 
- This computer supports whole numbers and chars, has no support in real numbers, aritmetic based on the complement to 2.

### Machine Instructions Structure
| 11, 10, 9, 8 | 7, 6, 5, 4 | 3, 2 | 1, 0 |
| --- | --- | --- | --- |
| opcode | funct | source | destination |

#### opcode table
| name | funct | opcode |
| --- | --- | --- |
| mov |  | 0 |
| cmp |  | 1 |
| add | 10 | 2 |
| sub | 11 | 2 |
| lea |  | 4 |
| clr | 10 | 5 |
| not | 11 | 5 |
| inc | 12 | 5 |
| dec | 13 | 5 |
| jmp | 10 | 9 |
| bne | 11 | 9 |
| jsr | 12 | 9 |
| red |  | 12 |
| prn |  | 13 |
| rts |  | 14 |
| stop |  | 15 |

### Addressing Modes
Addressing modes are the different ways to pass machine instructions opcodes.\
It specifies a rule for interpreting or modifying the address field of the instruction before the operand is executed.\
In this imaginary computer there are 4 addressing modes:
- immediate addressing mode.
- direct addressing mode.
- relative addressing mode.
- register direct addressing mode.

### Directive Sentences
Each directive will start with '.'
There are 4 directives:
- **'.data'** - For positive(+) or Negative(-) whole number seperated by ','.\
Instructs the Assembler to to allocate space in the data image and increment the data counter accordingly.
- **'.string'** - For ascii based string with '\0' null termination.
- **'.entry'** - Receives a label name which is defined in the current source file.\
For using the label as an instruction opcode.
- **'.extern'** - Receives a label name that is not defined in the current source file.\
Notifying the Assembler that the label is defined in another source file.

### Assembler Basic Algorithm
The machine codes is divided into 2 sections:
- IC -> Instruction Counter.
- DC -> Data Counter.

**The machine code can be uploaded into memory from address 100.**

**First Pass:**
1. Initialize DC -> 0, IC -> 100.
2. Reads the next line from source file, else jump to 17.
3. Is the first line field is a label? else jump to 5.
4. Raise flag "symbol".
5. Is this directive **'.data'** or **'.string'**? else jump to 8.
6. If a label, insert to symbol table with **'.data'** directive, increment DC. else print error.
7. Identify data type, encode to data image, increment DC with data length. jump to 2.
8. Is this directive **'.extern'** or **'.entry'**? else jump to 11.
9. Is this directive **'.entry'**? jump to 2.
10. Is this directive **'.extern'**? insert symbol into symbol table with the value of 0 and with **'.extern'** characteristic. else print error. jump to 2.
11. Instruction line, if this is a symbol, insert symbol into symbol table with 'code' characteristic, it's value will be the value of IC. else print error.
12. Look for opcode in opcode table, if not found print error with the opcode name.
13. Analyze the instruction opcodes structure, calculate the total amount of words for the machine code instructions, we will call this number L.
14. Build the binary code for the first instruction word, and all data-word encodes opcode with immediate addressing mode.
15. Save IC and L values with the machine code instruction data.
16. Update IC + L --> IC. jump to 2.
17. Source code has been read in full, if there were errors in first pass stop here.
18. Save IC and DC final values, we will call them ICF and DCF.
19. Update each symbol with **'.data'** characteristic in symbol table with ICF value.
20. Start second pass.

**Second Pass:**
1. Reads the next line from source file, else jump to 7.
2. If the first line field is a label? skip it.
3. Is this directive **'.data'** or **'.string'** or **'.extern'**? if true, jump to 1.
4. Is this directive **'.entry'**? if false, jump to 6.
5. Insert into symbol table the characteristic **'.entry'** to the symbol shows as the instruction opcode. else print error. jump to 1.
6. Complete the binary encoding of opcodes data-words according to the addressing modes being used.\
For each opcode in the source code that containing a symbol, find it's value in the symbol table, else print error.\
If the symbol characteristic is **'.external'** add the relevent data-word address to the list od information words that aremapped to an external symbol. jump to 1.
7. Source code has been read in full, if there were errors in second pass stop here.
8. Build output files.
