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

instruction * get_instructions(FILE * fp);

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

	if (argc > 1)
	{
		fp = fopen(argv[1], "r");
		get_instructions(fp);
	}
	else
	{
		printf("Error: invalid input file.\n");
		return 0;
	}


	printf("%d \n", atoi("15"));
	// create an array of instructions

	fclose(fp);

	return 0;
}

instruction * get_instructions(FILE * fp)
{
	int bufferLen = 16; // guessing max character length of an instruction
	char * buffer = malloc(sizeof(char) * bufferLen);

	instruction instructions[256]; // 256 is arbitrary
	int i = 0; // index counter for instruction array

	const char * delimiter =  " ";
	char * token;

	while(fgets(buffer, bufferLen, fp))
	{
		// grab first token
		int k = 0;
		token = strtok(buffer, delimiter);
		//printf("%d: ", i);
		while(token != NULL)
		{
			if (k == 0)
			{
				instructions[i].OP = atoi(token);
				printf("OP: %d\t", instructions[i].OP);
			}
			else if (k == 1)
			{
				instructions[i].R = atoi(token);
				printf("R: %d\t", instructions[i].R);
			}
			else if (k == 2)
			{
				instructions[i].L = atoi(token);
				printf("L: %d\t", instructions[i].L);
			}
			else if (k == 3)
			{
				instructions[i].M = atoi(token);
				printf("M: %d\t", instructions[i].M);
				printf("\n");
				break;
			}

			token = strtok(NULL, delimiter);

			k++;
		}

		i++;
		// for testing purposes
		if (i == 5)
				break;
	}


	/*
	for (int q = 0; q < i; q ++)
	{
		printf("%d\t%d\t%d\t%d\n", instructions[i].OP, instructions[i].R,instructions[i].L, instructions[i].M);
	}
	*/
	return NULL;
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
