## Introduction

This project simulates a full compiler for the PL/0 programming language, written entirely in C.

The code is based on a class project from my Systems Software class when I was in college.

See HW1-VirtualMachine.doc, HW2-LexScanner.doc, and HW3-ParserandCodeGenerator.doc for more information.

## PL-0-Compiler
A PL/0 compiler with lexical analyzer, syntax analyzer and machine code generator with a PM/0 virtual machine containing features designed to support function and procedure calls with parameter passing in C.

## Usage
Copy the project using `git clone  https://github.com/cwalk/PL0-Compiler`

Navigate to the directory containing the source code and the test files. `cd compiler`

- Run the following command to compile:

$ gcc -o [Executable File Name] *.c

Then to run the executable file:

$ ./[Executable File Name] [Command line switches]

The following command line switches print its corresponding
type of output to the screen:

  -l  The lexeme list created
  -a 	The disassembled code from the code generator
  -v 	The virtual machine execution stack trace

Make sure the test files are in the same directory as the source files (or in the home directory for OSX).

##README

The project folder should be named "compiler" Inside you should find the following: 

	-data.h (a header file that contains defines, variables, and functions used across all .c files. All c files include this header file in their respective source codes)
	-main.c (a C program that has a main function. This will run all the other C files: lex, parser, and vm, in that order. It will also take care of the -l -a -v commands)
	-lex.c (the lex/scanner)
	-parser.c (the parser/intermediate code generator)
	-vm.c (the virtual machine)
	-a folder called "test files"


Inside the "test files" folder, there will be 5 more folders:

	-The first is called "input test files" and holds 12 different input test cases.
	
	-The next folder "lex output files" contains the output files from lex, which uses the 12 input test cases as its input files. These files will have the lexeme list after lex runs with the input files.

	-The next folder "parser output files" contains the output files from the parser, which uses the 12 lex output files as its input files. These files have the intermediate generated code after parser runs with the lex output file as its input.

	-The next folder "vm output files" contains the output files from the vm, which uses the 12 parser output files as its input files. These files have the instructions printed in order, and then the stack frame of the instructions being executed, after the vm runs with the parser output file as its input.

	-The last folder "all errors output" contains a single file called "errors" which holds all the errors that the program might produce. Note: Not all of these error messages may be used.

******************************************************************************************

When you use the ./compile command, the program will print "What is the name of your test file?" to the screen, and if you type "input.txt" it will run fine. 

The program works like this: After you compile and run, main.c runs first. Inside main.c, it will take care of the "-l -a -v" commands. It will then run lex.c.

lex.c will use the "input.txt" passed in from the user as its input file, and will generate an output file called "lexoutput.txt" which holds the lexeme list. Then, parser.c will be run.

parser.c will use the "lexoutput.txt" generated from the lex.c as its input file, and will generate an output file called "parseroutput.txt" which holds the generated code. Then, vm.c will be run.

vm.c will use the "parseroutput.txt" generated from the parser.c as its input file, and will generate 3 output files called "vmout.txt" which holds both the instructions and stack frame, "vmout2.txt" which holds just the instructions,  and "vmout3.txt" which holds just the stack frame. 

Then, main.c will print the lexeme list, generated code, and stack frame, depending on the "-l -a -v" commands called from the user during initial execution. main.c will also print out if there were no errors.


An example of compiling would look like this:

./compile -l -a -v         

This would print out all 3: lexeme list, generated code, and stack frame, to the console.
******************************************************************************************

You can compile and run your program, using the commands below: 

To compile: gcc –o <runfile> <filename.c>
To Run: ./<runfile>

Then, the program will prompt you with what input.txt you wish to run. Examples would be "input.txt" or "input1.txt" etc.

A full example would be to name the runfile "compile" as this would make the command something similar to this:

To compile: gcc –o compile main.c lex.c parser.c vm.c
To Run: ./compile

To print out different things: ./compile -l -a -v

You will then be prompted for the input file name. The input file must be in the same directory as the source files (or the home directory in OSX). 

Be sure to check if your input file does a read or a write. If the program is stopped but not crashed, it might be waiting for a scanf, meaning you called read in the test input file. If a single number is printed to screen then it is the result of the write function. 

write will write things to screen right after the program prompts the user for the input file name. If "-l -a -v" commands were used, they would be printed after the write functions printed their variables. 
******************************************************************************************

This compiler project follows the specifications and EBNF tree specified in the HW3 doc from COP 3402 Summer 2013. PL/0 is a simpler programming language than Pascal. It serves as an example of how to construct a compiler. This compiler is a recursive descent parser with a simple syntax. The grammar can be viewed in the EBNF tree in the HW3 doc. 
