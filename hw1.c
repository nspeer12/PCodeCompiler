#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct IR
{
	int OP;
	int * ADDR;
};

typedef struct instruction{
	int OP;
	int R;
	int L;
	int M;
} instruction;

static int MAX_DATA_STACK_HEIGHT = 40;
static int MAX_CODE_LENGTH = 500;
static int MAX_LEXI_LEVELS = 3;

instruction ** get_instructions(FILE * fp);
void print_code(instruction ** code, int codeLen, int * BP, int * SP, int * GP, int * PC, int * IR);

int main(int argc, char *argv[])
{
	// per instructions

	int * data_stack[MAX_DATA_STACK_HEIGHT];
	for (int i = 0; i < MAX_DATA_STACK_HEIGHT; i++)
		data_stack[i] = 0;

	int * SP = malloc(sizeof(int));
	* SP = MAX_DATA_STACK_HEIGHT;

	int * BP = malloc(sizeof(int)); // base pointer
	* BP = 0;

	int * GP = malloc(sizeof(int)); // global pointer
	* GP = -1;

	int * PC = malloc(sizeof(int));
	* PC = 0;

	int * IR = malloc(sizeof(int));
	* IR = 0;

	int * stack = calloc(sizeof(int), MAX_DATA_STACK_HEIGHT);
	
	// digest input file and store in an array of instructions
	FILE * fp;
	instruction ** code;

	if (argc > 1)
	{
		fp = fopen(argv[1], "r");
		code = get_instructions(fp);
	}
	else
	{
		printf("Error: invalid input file.\n");
		return 0;
	}
	fclose(fp);

	print_code(code, 17, BP, SP, GP, PC, IR);

	return 0;
}

instruction ** get_instructions(FILE * fp)
{
	int bufferLen = 16; // guessing max character length of an instruction
	char * buffer = malloc(sizeof(char) * bufferLen);

	// instruction instructions[256]; // 256 is arbitrary
	instruction ** code = malloc(sizeof(instruction *) * MAX_CODE_LENGTH);
	int i = 0; // index counter for instruction array

	const char * delimiter =  " ";
	char * token;

	while(fgets(buffer, bufferLen, fp))
	{
		// grab first token
		int k = 0;
		token = strtok(buffer, delimiter);

		// I'm just going to keep it stupid simple and use a local variable.
		// Trying to work with a double pointer matrix gave me 23 compiler errors and an aneurysm
		instruction  * curInst = malloc(sizeof(instruction));
		code[i] = curInst;
		//printf("%d: ", i);
		while(token != NULL)
		{
			if (k == 0)
			{
				curInst->OP = atoi(token);
			}
			else if (k == 1)
			{
				curInst->R = atoi(token);
			}
			else if (k == 2)
			{
				curInst->L = atoi(token);
			}
			else if (k == 3)
			{
				curInst->M = atoi(token);
				break;
			}

			token = strtok(NULL, delimiter);

			k++;
		}

		i++;
		// prevent stack from overflowing
		if (i == MAX_CODE_LENGTH)
				break;
	}

	
	return code;
}

void print_code(instruction ** code, int codeLen, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	printf("Line OP R L M\n");
	for (int i=0; i<codeLen; i++)
	{
		printf("%-5d", i);
		switch (code[i]->OP)
		{	
			case(0):
				// loads a constant value
				printf("LIT %.2d, 0, %d\n", code[i]->R, code[i]->M);
				break;
			case(1):
				// returns from a subroutine
				printf("RTN 0, 0, 0\n");
				break;
			case(2):
				// loads the value into a register from a stack location at offset M and level L
				printf("LOD %d, %d, %d\n",code[i]->R, code[i]->L, code[i]->M);
				break;
			case(3):
				// store the value in the stack at offset M and level L
				printf("STO %d, %d, %d\n",code[i]->R, code[i]->L, code[i]->M);
				break;
			case(4):
				// call a procedure at index M
				// generates a new activation record
				printf("CAL 0, %d, %d\n", code[i]->L, code[i]->M);
				break;
			case(5):
				// allocate M locals, increment stack pointer by M
				// first four values are functional value, static link, dyn. link, and return addr.
				printf("INC 0, 0, %d\n", code[i]->M);
				break;
			case(6):
				// jump to instruction M
				printf("JMP 0, 0, %d\n", code[i]->M);
				break;
			case(7):
				// jump to instruction M if R == 0
				printf("JPC %d, 0, %d\n", code[i]->R, code[i]->M);
				break;
			case(8):
				// write a register to the screen
				printf("SIO %d, 0, 1\n", code[i]->R);
				break;
			case(9):
				// read input from the user and store it in the register
				printf("SIO %d, 0, 2\n", code[i]->R);
				break;
			case(10):
				// set halt flag to 1, end of program
				printf("SIO 0, 0, 3\n");
				break;
			case(12):
				// negate
				break;
			default:
				break;
		}
	}
}

int fetch(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	// code[PC] -> IR
	return 0;
}

int execute(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{

	return 0;
}

void LIT(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	;
}

void RTN(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	;
}

void LOD(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	;
}

void STO(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	;
}

void CAL(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	;
}

void INC(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	;
}

void JMP(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	;
}

void JPC(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	;
}

void SIO_0(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	;
}

void SIO_1(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	;
}
void SIO_2(instruction ** code, int * BP, int * SP, int * GP, int * PC, int * IR)
{
	;
}
