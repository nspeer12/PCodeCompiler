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
void print_code(instruction ** code, int codeLen, int * BP, int * SP, int * PC, instruction * IR);
void fetch(instruction ** code, int * BP, int * SP, int * PC, instruction * IR);
int base(instruction * IR, int * stack, int * BP);
void print_state(instruction * IR, int * stack, int * PC, int * BP, int * SP, int * registerFile);
int execute(instruction ** code, int * BP, int * SP, int * PC, instruction * IR, int * stack, int * registerFile);

int main(int argc, char *argv[])
{

	int * SP = malloc(sizeof(int));
	* SP = 0;

	int * BP = malloc(sizeof(int)); // base pointer
	* BP = 1;

	int * PC = malloc(sizeof(int));
	* PC = 0;

	instruction * IR = malloc(sizeof(instruction));
	IR->OP = 0;
	IR->R = 0;
	IR->L = 0;
	IR->M = 0;

	int * stack = calloc(sizeof(int), MAX_DATA_STACK_HEIGHT);
	
	int * registerFile = calloc(sizeof(int), 8);

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

	//print_code(code, 17, BP, SP, PC, IR);

	for (int i = 0; i < 10; i ++)
	{
		fetch(code, BP, SP, PC, IR);
		execute(code, BP, SP, PC, IR, stack, registerFile);

		// printf("%d\n", code[i]->OP);
	}
	

	return 0;
}

int execute(instruction ** code, int * BP, int * SP, int * PC, instruction * IR, int * stack, int * registerFile)
{
		// i is the R operand
		
		// !!! Check the condition for the switch
		switch (IR->OP)
		{	
			case(0):
				// loads a constant value
				// printf("LIT %.2d, 0, %d\n", code[i]->R, code[i]->M);
				registerFile[IR->R] = IR->M;
				break;

			case(1):
				// returns from a subroutine
				// printf("RTN 0, 0, 0\n");
				*SP = *BP - 1;
				*BP = stack[*SP + 3];
				*PC = stack[*SP + 4];
				break;

			case(2):
				// loads the value into a register from a stack location at offset M and level L
				// printf("LOD %d, %d, %d\n",code[i]->R, code[i]->L, code[i]->M);
				IR->R = stack[ base(IR, BP, stack) + IR->M];
				break;

			case(3):
				// store the value in the stack at offset M and level L
				// printf("STO %d, %d, %d\n",code[i]->R, code[i]->L, code[i]->M);
				registerFile[IR->R] = stack[ base(IR, stack, BP) + IR->M];
				break;

			case(4):
				// call a procedure at index M
				// generates a new activation record
				// printf("CAL 0, %d, %d\n", code[i]->L, code[i]->M);
				stack[*SP + 1] = 0;	// space return value
				stack[*SP + 2] = base(IR, stack, BP); 	// static link (SL)
                stack[*SP + 3] = *BP;	// dynamic link (DL)
	           	stack[*SP + 4] = *PC;	 		// return address (RA) 
                *BP = *SP + 1;
	          	*PC = IR->M;
				break;

			case(5):
				// allocate M locals, increment stack pointer by M
				// first four values are functional value, static link, dyn. link, and return addr.
				printf("INC 0, 0, %d\n", code[IR->R]->M);
				*SP = *SP + IR->M;
				break;

			case(6):
				// jump to instruction M
				printf("JMP 0, 0, %d\n", code[IR->R]->M);
				*PC = IR->M;
				break;

			case(7):
				// jump to instruction M if R == 0
				printf("JPC %d, 0, %d\n", code[IR->R]->R, code[IR->R]->M);
				if (registerFile[IR->M] == 0)
					*PC = IR->M;
				break;

			case(8):
				// write a register to the screen
				printf("SIO %d, 0, 1\n", code[IR->R]->R);
				printf("%d", registerFile[IR->R]);
				break;

			case(9):
				// read input from the user and store it in the register
				printf("SIO %d, 0, 2\n", code[IR->R]->R);
				scanf("%d", &registerFile[IR->R]);
				break;

			case(10):
				// set halt flag to 1, end of program
				printf("SIO 0, 0, 3\n");
				return 1;
				break;

			default:
				break;
		}

		print_state(IR, stack, PC, BP, SP, registerFile);
	return 0;
}


void fetch(instruction ** code, int * BP, int * SP, int * PC, instruction * IR)
{

	printf("PC: %d\n", *PC);
	*IR = *code[*PC];
	*PC = *PC + 1;
	return;
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



void print_state(instruction * IR, int * stack, int * PC, int * BP, int * SP, int * registerFile)
{
	printf("PC: %d\tBP: %d\tSP: %d\tregisters: ", *PC, *BP, *SP);
	for (int i = 0; i < 8; i ++)
		printf("%d ", registerFile[i]);
	printf("\n");
	return;
}
int base(instruction * IR, int * stack, int * BP)
{
	// defined in instructions

	int b = *BP;
	while(IR->L > 0)
	{
		b = stack[b + 1];
		IR->L --;
	}
	return b;
}

void print_code(instruction ** code, int codeLen, int * BP, int * SP, int * PC, instruction * IR)
{
	printf("Line OP R L M\n");
	int i;
	for (i=0; i<codeLen; i++)
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
			default:
				break;
		}
	}

	return;
}
