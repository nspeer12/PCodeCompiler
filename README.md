# PCodeCompiler

# Getting Started
In order to run the compile script, you may need to grant it executable permissions
`chmod +x compile.sh`

### How To Run
run in default mode
`./compile.sh` or `bash compile.sh`

default mode runs the file input/input01.txt


## Usage

**note** <filename> is inputs/input0.txt

print the list of lexemes/tokens to the screen

`./compile.sh -l <filename>`

print the generated assembly code to the screen

`./compile.sh -a <filename>`

print the virtual machine execution trace

`./compile.sh -v <filename>`

or use in combination

`./compile.sh -l -v -a <filename>`

## Lost?

`./compile.sh --help`

or

`./compile.sh -h`


## "Lex" or Lexicographical Analyzer

Lex is a tool that take an PL/0 input file makes tokens and identifies different syntax

To run individually

`gcc lex.c` and `./a.out -l <inputfile>`

## Parser
Parser scans through lexemes and identifies patterns in the code

`gcc parser.c`

`./a.out -l <inputfile`


## VM

VM is a virtual machine that runs compiled PL/0 code

`gcc vm.c`

`./a.out -v <compiled-code>`
