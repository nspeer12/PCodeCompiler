#!/bin/bash


# input file is the last argument

# TODO fix input



# take command line arguments
LEX=0
ASSEMBLY=0
VM=0
HELP=0
DEBUG=0

# default file
file="inputs/input01.txt"

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

		  -h)
		  HELP=1
		  ;;
		  --help)
		  HELP=1
		  ;;
		  --debug)
		  DEBUG=1
		  ;;

		  *)
		  	# assume anything else is the input file
	  		file=$arg
			;;
	 esac
done


if [[ $DEBUG = "1" ]]
then
	echo $file
	#debug
fi

# Need to change to True/False instead of 0 or 1
#echo "# Output Lex: $LEX"
#echo "# Output Assembly: $ASSEMBLY"
#echo "# Execute on VM: $VM"
if [ $HELP = "1" ]
then
	echo "PL/0 Compiler"
	echo "proper syntax: ./compile.sh <arguments> <inputfile>"
	echo "-l: print the list of lexemes/tokens to the screen"
	echo "-a: print the generaged assembly code to the screen"
	echo "-v: print the virtual machine execution trace to the screen"
	exit 1
fi


# compile Lex and input file and output to temporary location to feed to Parser
gcc lex.c -o lex.o

# hardcode lex input for now
if [ $LEX = "1" ]
then
	./lex.o $file -l
else
	./lex.o $file
fi

rm "lex.o"

# compile parser
# TODO: handle input arguments
gcc parser.c -o parser.o

if [ $ASSEMBLY = "1" ]
then
	./parser.o -a
else
	./parser.o
fi

rm parser.o

if [ $VM = "1" ]
then
	gcc "vm.c" -o vm.o
	./vm.o $file
	rm vm.o
fi


echo # extra space ;)
