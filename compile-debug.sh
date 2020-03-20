#!/bin/bash

execute ()
{
	# compile Lex and input file and output to temporary location to feed to Parser
	gcc lex/lex.c -o lex.o

	# hardcode lex input for now
	if [ $LEX = "1" ]
	then
		./lex.o lex/input03.txt -l
	else
		./lex.o lex/input03.txt
	fi

	# compile parser
	# TODO: handle input arguments
	gcc parser/parser.c -o parser.o

	if [ $ASSEMBLY = "1" ]
	then
		./parser.o -a
	else
		./parser.o
	fi

	if [ $VM = "1" ]
	then
		gcc "vm/vm.c" -o vm.o
		./vm.c $file
	fi
}

run ()
{
	# input file is the last argument
	if [[ "$1" -ne 1 ]]
	then
		# set default filepath
		file="lex/input01.txt"
	else
		file="{@:-1}"
	fi


	# take command line arguments
	LEX=0
	ASSEMBLY=0
	VM=0
	HELP=0
	DEBUG==0

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
	    esac
	done

	if [[ $DEBUG = "1" ]]
	then
		debug
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

	execute
}


debug()
{
	lex_input="l"
	vm_input="v"
	assembly_input="a"


	if [ $DEBUG = "1" ]
	then
		while [ $DEBUG = "1" ]
		do
			run
			echo "enter flags and filename"
			read input

			if [[ "$input" == *"l"* ]];
			then
	  			LEX=1
			else
				LEX=0
			fi
			if [[ "$input" == *"a"* ]]
			then
	  			ASSEMBLY=1
			else
				ASSEMBLY=0
			fi
			if [[ "$input" == *"v"* ]]
			then
	  			VM=1
			else
				VM=0
			fi
			execute
		done
	fi
}


run
