#!/bin/bash

# take command line arguments

LEX=0
ASSEMBLY=0
VM=0

for arg in "$@"
do
    case $arg in
        -l)
       	LEX=1
        ;;

        -a)
        ASSEMBLY=1
        ;;

        -v)
        VM=1
        ;;
    esac
done

True = "true"
False = "false"

# Need to change to True/False instead of 0 or 1
echo "# Output Lex: $LEX"
echo "# Output Assembly: $ASSEMBLY"
echo "# Execute on VM: $VM"


# compile Lex and input file and output to temporary location to feed to Parser
gcc lex/lex.c -o lex.o

# hardcode lex input for now
./lex.o lex/input01.txt

# compile parser
# TODO: handle input arguments
gcc parser/parser.c -o parser.o

# hard code the -l flag for now
./parser.o -l
