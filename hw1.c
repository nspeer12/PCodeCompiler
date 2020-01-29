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

static int MAX_DATA_STACK_HEIGHT = 23;
static int MAX_CODE_LENGTH = 500;
static int MAX_LEXI_LEVELS = 3;

instruction ** get_instructions(FILE * fp);

int main(int argc, char *argv[])
{
	// per instructions

	int * data_stack[MAX_DATA_STACK_HEIGHT];
	for (int i = 0; i < MAX_DATA_STACK_HEIGHT; i++)
		data_stack[i] = 0;

	int SP = MAX_DATA_STACK_HEIGHT;
	int BP = 0; // base pointer
	int GP = -1; // global pointer
	int PC = 0;
	int IR = 0;

	// digest input file
	FILE * fp;

	// return varaible for testing
	instruction ** insts;

	if (argc > 1)
	{
		fp = fopen(argv[1], "r");
		insts = get_instructions(fp);
	}
	else
	{
		printf("Error: invalid input file.\n");
		return 0;
	}

	printf("%d\t%d\t%d\t%d\n", insts[0]->OP, insts[0]->R, insts[0]->L, insts[0]->M);
	// create an array of instructions

	fclose(fp);

	return 0;
}

instruction ** get_instructions(FILE * fp)
{
	int bufferLen = 16; // guessing max character length of an instruction
	char * buffer = malloc(sizeof(char) * bufferLen);

	// instruction instructions[256]; // 256 is arbitrary
	instruction ** insts = malloc(sizeof(instruction *) * 256);
	int i = 0; // index counter for instruction array

	const char * delimiter =  " ";
	char * token;

	while(fgets(buffer, bufferLen, fp))
	{
		// grab first token
		int k = 0;
		token = strtok(buffer, delimiter);

		instruction curInst;
		insts[i] = &curInst;
		//printf("%d: ", i);
		while(token != NULL)
		{
			if (k == 0)
			{
				curInst.OP = atoi(token);
			}
			else if (k == 1)
			{
				curInst.R = atoi(token);
			}
			else if (k == 2)
			{
				curInst.L = atoi(token);
			}
			else if (k == 3)
			{
				curInst.M = atoi(token);
				break;
			}

			token = strtok(NULL, delimiter);

			k++;
		}

		i++;
		// for testing purposes
		if (i == 10)
				break;
	}


	/*
	for (int q = 0; q < i; q ++)
	{
		printf("%d\t%d\t%d\t%d\n", instructions[i].OP, instructions[i].R,instructions[i].L, instructions[i].M);
	}
	*/
	return insts;
}

int fetch()
{
	// code[PC] -> IR
	return 0;
}

int execute()
{
	// execute the code from fetch cycle
	return 0;
}

void LIT()
{
	;
}

void RTN()
{
	;
}

void LOD()
{
	;
}

void STO()
{
	;
}

void CAL()
{
	;
}

void INC()
{
	;
}

void JMP()
{
	;
}

void JPC()
{
	;
}

void SIO_0()
{
	;
}

void SIO_1()
{
	;
}
void SIO_2()
{
	;
}
