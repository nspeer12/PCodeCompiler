// Created By:
// Stephen Speer
// Danish Siddiqui

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct IR
{
	int OP;
	int * ADDR;
};

// Defining the instruction.
typedef struct instruction{
	int OP;
	int R;
	int L;
	int M;
} instruction;

static int MAX_DATA_STACK_HEIGHT = 40;
static int MAX_CODE_LENGTH = 500;
static int MAX_LEXI_LEVELS = 3;

instruction * get_instructions(FILE * fp, int numLines);
void print_code(instruction * code, int codeLen, int * stack, int * registers, int PC, int BP, int SP);
int base(int l, int * stack, int BP);
void print_state(instruction IR, int * stack, int PC, int BP, int SP, int * registerFile, int * activationRecord);

int main(int argc, char *argv[])
{
	int print = (argc > 2 && (strcmp(argv[2], "-v") == 0)) ? 1 : 0;

	// Stack Pointer
	// Points to the top of the stack.
	int SP = 3;

	// Base Pointer
	// Points to a current AR in stack.
	int BP = 0;

	// Program Counter
	int PC = 0;

	// setting defaults of ISA
	instruction IR;
	IR.OP = 0;
	IR.R = 0;
	IR.L = 0;
	IR.M = 0;

	// creating the stack memory as an array initializing all values to 0
	int * stack = malloc(MAX_DATA_STACK_HEIGHT * sizeof(int));
	for (int i = 0; i < MAX_DATA_STACK_HEIGHT; i++)
		stack[i] = 0;

	// boolean array for storing positions of the activation record
	int * activationRecord = malloc(MAX_DATA_STACK_HEIGHT * sizeof(int));
	for (int i = 0; i < MAX_DATA_STACK_HEIGHT; i++)
		stack[i] = 0;

	// create register file of length 8
	int * registerFile = malloc(8 * sizeof(int));
	for (int i = 0; i < 8; i++)
		registerFile[i] = 0;

	// retreiving data from input file.
	instruction * code;
	int numLines = 0;

	// File IO
	if (argc > 1)
	{
		FILE * fp;
		// count length of file
		char ch;
		fp = fopen(argv[1], "r");
		while(!feof(fp))
		{
  			ch = fgetc(fp);
  			if(ch == '\n')
    			numLines++;
		}
		// close and reopen file so we can start from the beggining of the file
		fclose(fp);
		fp = fopen(argv[1], "r");

		// function that retreives data from the input file
		code = get_instructions(fp, numLines);

		if (print == 1)
			printf("NUM LINES %d\n", numLines);

		fclose(fp);
	}
	else
	{
		printf("Error: invalid input file.\n");
		return 0;
	}


	// Putting numLines-1 because it has 17 lines but it only goes up to 16 in the output.
	// The 17th line is empty.
	if (print)
		print_code(code, numLines-1, stack, registerFile, PC, BP, SP);

	// Fetching and executing functions happen
	int halt = 0;

	while(!halt)
	{
		IR = code[PC];

		if (print == 1)
			printf("%d ", PC);
		PC = PC + 1;
		// printf("\nOP: %d\tR %d\tL %d\tM %d\n", IR.OP, IR.R, IR.L, IR.M);

		//print_state(IR, stack, PC, BP, SP, registerFile, activationRecord);
		switch (IR.OP)
		{
			case(1):
				// loads a constant value
				if (print == 1)
					printf("lit %d 0 %d ", IR.R, IR.M);
				registerFile[IR.R] = IR.M;
				break;

			case(2):
				// returns from a subroutine
				if (print == 1)
					printf("rtn 0 0 0 ");
				SP = BP - 1;
				BP = stack[SP + 3];
				PC = stack[SP + 4];
				break;

			case(3):
				// loads the value into a register from a stack location at offset M and level L
				if (print == 1)
					printf("lod %d %d %d ",IR.R, IR.L, IR.M);
				registerFile[IR.R] = stack[base(IR.L, stack, BP) + IR.M];
				break;

			case(4):
				// store the value in the stack at offset M and level L
				if (print == 1)
					printf("sto %d %d %d ",IR.R, IR.L, IR.M);
				stack[base(IR.L, stack, BP) + IR.M] = registerFile[IR.R];
				break;

			case(5):
				// call a procedure at index M
				// generates a new activation record
				activationRecord[SP] = 1;
				activationRecord[0] = 0;
				if (print == 1)
					printf("cal 0 %d %d ", IR.L, IR.M);
				stack[SP + 1] = 0;								// space return value
				stack[SP + 2] = base(IR.L, stack, BP); 	// static link (SL)
				stack[SP + 3] = BP;								// dynamic link (DL)
				stack[SP + 4] = PC;	 							// return address (RA)
				BP = SP + 1;
				SP = BP + 4;
				PC = IR.M;

				break;

			case(6):
				// allocate M locals, increment stack pointer by M
				// first four values are functional value, static link, dyn. link, and return addr.
				if (print == 1)
					printf("inc 0 %d %d ", IR.L, IR.M);
				SP = SP + IR.M;
				break;

			case(7):
				// jump to instruction M
				if (print == 1)
					printf("jmp 0 0 %d ", IR.M);
				PC = IR.M;
				break;

			case(8):
				// jump to instruction M if R == 0
				if (print == 1)
					printf("jpc %d, 0, %d ", IR.R, IR.M);
				if (registerFile[IR.R] == 0)
					PC = IR.M;
				break;

			case(9):
				// write a register to the screen
				if (print == 1)
					printf("sio %d, 0, 1 ", IR.R);

				printf("%d\n", registerFile[IR.R]);
				break;

			case(10):
				// read input from the user and store it in the register
				if (print == 1)
					printf("sio %d, 0, 2 ", IR.R);
				int reg = IR.R;
				scanf("%d", &registerFile[reg]);
				break;

			case(11):
				// set halt flag to 1, end of program
				if (print == 1)
					printf("sio 0, 0, 3 ");
				halt = 1;
				break;

			case(12):
				// NEG, just multiplies R[i] * -1
				registerFile[IR.R] = registerFile[IR.R] * -1;
				break;

			case(13):
				// ADD R[i] = R[j] + R[k], j refers to "L", k refers to "M".
				registerFile[IR.R] = registerFile[IR.L] + registerFile[IR.M];
				break;

			case(14):
				// Subtract
				registerFile[IR.R] = registerFile[IR.L] - registerFile[IR.M];
				break;

			case(15):
				// Multiply
				registerFile[IR.R] = registerFile[IR.L] * registerFile[IR.M];
				break;

			case(16):
				// Divide
				registerFile[IR.R] = registerFile[IR.L] / registerFile[IR.M];
				break;

			case(17):
				// Check odd value.
				registerFile[IR.R] = registerFile[IR.R] % 2;
				break;

			case(18):
				// Check odd value.
				registerFile[IR.R] = registerFile[IR.L] % registerFile[IR.M];
				break;

			case(19):
				// Check for equivalence.
				registerFile[IR.R] = (registerFile[IR.L] == registerFile[IR.M]);
				break;

			case(20):
				// NEQ.
				registerFile[IR.R] = (registerFile[IR.L] != registerFile[IR.M]);
				break;

			case(21):
				// LSS.
				registerFile[IR.R] = (registerFile[IR.L] < registerFile[IR.M]);
				break;

			case(22):
				// LEQ.
				registerFile[IR.R] = (registerFile[IR.L] <= registerFile[IR.M]);
				break;

			case(23):
				// GTR.
				registerFile[IR.R] = (registerFile[IR.L] > registerFile[IR.M]);
				break;

			case(24):
				// GEQ.
				registerFile[IR.R] = (registerFile[IR.L] >= registerFile[IR.M]);
				break;

			default:
				break;
		}

		if (IR.OP == 2)
			activationRecord[SP] = 0;



		if (print)
			print_state(IR, stack, PC, BP, SP, registerFile, activationRecord);

		if (PC > numLines)
			halt=1;
	}

	printf("\n");
	return 0;
}

instruction * get_instructions(FILE * fp, int numLines)
{
	// guessing max character length of an instruction
	int bufferLen = 16;
	char * buffer = malloc(sizeof(char) * bufferLen);

	// instruction instructions[256];
	// 256 is arbitrary
	instruction * code = malloc(sizeof(instruction) * numLines);

	// index counter for instruction array
	int i = 0;

	const char * delimiter =  " ";
	char * token;

	while(i < numLines)
	{
		fgets(buffer, bufferLen, fp);
		// grab first token
		int k = 0;
		token = strtok(buffer, delimiter);

		// I'm just going to keep it stupid simple and use a local variable.
		// Trying to work with a double pointer matrix gave me 23 compiler errors and an aneurysm
		if (i == numLines)
				break;

		//printf("%d: ", i);
		while(token != NULL)
		{
			if (k == 0)
			{
				code[i].OP = atoi(token);
			}
			else if (k == 1)
			{
				code[i].R = atoi(token);
			}
			else if (k == 2)
			{
				code[i].L = atoi(token);
			}
			else if (k == 3)
			{
				code[i].M = atoi(token);
				break;
			}

			token = strtok(NULL, delimiter);

			k++;
		}

		i++;
	}

	return code;
}



void print_state(instruction IR, int * stack, int PC, int BP, int SP, int * registerFile, int * activationRecord)
{
	printf("\t%d\t%d\t%d\t", PC, BP, SP);
	for (int i = 0; i < 8; i ++)
		printf("%d ", registerFile[i]);

	printf("\nStack: ");

	for (int i = 0; i <= SP; i ++)
	{
		printf("%d", stack[i]);
		if (activationRecord[i] == 1)
			printf("|");
		else
			printf(" ");
	}

	printf("\n\n");
	return;
}

int base(int l, int * stack, int BP)
{
	int b = BP;
	while(l > 0)
	{
		b = stack[b + 1];
		l --;
	}
	return b;
}

void print_code(instruction * code, int codeLen, int * stack, int * registers, int PC, int BP, int SP)
{
	printf("Line\tOP\tR\tL\tM\n");
	int i;
	for (i=0; i <= codeLen; i++)
	{
		switch (code[i].OP)
		{
			case(1):
				// loads a constant value
				printf("%d\tlit\t%d\t0\t%d\n", i, code[i].R, code[i].M);
				break;
			case(2):
				// returns from a subroutine
				printf("%d\trtn\t0\t0\t0\n", i);
				break;
			case(3):
				// loads the value into a register from a stack location at offset M and level L
				printf("%d\tlod\t%d\t%d\t%d\n", i, code[i].R, code[i].L, code[i].M);
				break;
			case(4):
				// store the value in the stack at offset M and level L
				printf("%d\tsto\t%d\t%d\t%d\n", i, code[i].R, code[i].L, code[i].M);
				break;
			case(5):
				// call a procedure at index M
				// generates a new activation record
				printf("%d\tcal\t0\t%d\t%d\n",i, code[i].L, code[i].M);
				break;
			case(6):
				// allocate M locals, increment stack pointer by M
				// first four values are functional value, static link, dyn. link, and return addr.
				printf("%d\tinc\t0\t0\t%d\n", i, code[i].M);
				break;
			case(7):
				// jump to instruction M
				printf("%d\tjmp\t0\t0\t%d\n", i, code[i].M);
				break;
			case(8):
				// jump to instruction M if R == 0
				printf("%d\tjpc\t%d\t0\t%d\n", i, code[i].R, code[i].M);
				break;
			case(9):
				// write a register to the screen
				printf("%d\tsio\t%d\t0\t1\n", i, code[i].R);
				break;
			case(10):
				// read input from the user and store it in the register
				printf("%d\tsio\t%d\t0\t2\n", i, code[i].R);
				break;
			case(11):
				// set halt flag to 1, end of program
				printf("%d\tsio\t0\t0\t3\n", i);
				break;
			case(12):
				// set halt flag to 1, end of program
				///printf("%d\t\t0\t0\t3\n", i);
				break;
			case(13):
				// set halt flag to 1, end of program
				printf("%d\tadd\t%d\t%d\t%d\n", i, code[i].R, code[i].L, code[i].M);
				break;
			case(14):
				// set halt flag to 1, end of program
				printf("%d\tsub\t%d\t%d\t%d\n", i, code[i].R, code[i].L, code[i].M);
				break;
			default:
				break;
		}
	}

	printf("\t\tpc\tbp\tsp\tregisters\n");
	printf("Initial Values\t%d\t%d\t%d\t", PC, BP, SP);
	for(int i =0; i < 8; i++)
		printf("%d ", registers[i]);
	printf("\nStack: ");
	for(int i = 0; i < MAX_DATA_STACK_HEIGHT; i++)
		printf("%d ", stack[i]);
	printf("\n\n");
	printf("\t\tpc\tbp\tsp\tregisters\n");
	return;
}
