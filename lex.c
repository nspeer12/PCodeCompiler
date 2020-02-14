#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// REad a source porgram written in PL/0,
// identify errors
// output source program, lexeme table, and list of lexemes


typedef struct lexeme
{
		char * name;
		int type;
}lexeme;


void insertTable(lexeme ** lex, char * buffer, int k);
void printLexTable(lexeme ** lexTable, int k);

int main(int argc, char *argv[])
{
	// echo program
	// File IO
	FILE * fp;

	if (argc > 0)
	{
		fp = fopen(argv[1], "r");
	}

	int lexLen = 256;
	lexeme ** lexTable = malloc(sizeof(lexeme *) * lexLen);

	for (int i = 0; i < lexLen; i++)
	{
		lexTable[i] = malloc(sizeof(lexeme));
	}

	int k = 0; // keep track of lexeme table index

	int i = 0;
	char tmp;
	char * buffer = malloc(sizeof(char) * 64); // 63 charcter buffer

	while(!feof(fp))
	{
		tmp = fgetc(fp);

		if (tmp == ',')
		{
			insertTable(lexTable, buffer, k++);
			//insertTable(lexTable, ",", k++);
			i = 0;
		}
		else if (tmp == ' ')
		{
			insertTable(lexTable, buffer, k++);
			i = 0;
		}
		else if (tmp == ';')
		{
			insertTable(lexTable, buffer, k++);
			//insertTable(lexTable, ";", k++);
			i = 0;
		}
		else if (tmp == '\n')
		{
			//insertTable(lexTable, buffer, k++);
			i = 0;
		}
		else
		{
			buffer[i] = tmp;
			buffer[i+1] = '\0';
		}
		printLexTable(lexTable, k);
		printf("\n");
	}

	printLexTable(lexTable, 5);

	return 0;
}


void insertTable(lexeme ** lexTable, char * buffer, int k)
{
	// 12 character name, for now
	lexTable[k]->name = malloc(sizeof(char) * 12);
	// because strcpy is a steaming pile of shit
	strcpy(lexTable[k]->name, buffer);
	//printf("%s\n", lexTable[k]->name);
	// temporarily use this for now
	lexTable[k]->type = k;

	return;
}


void printLexTable(lexeme ** lexTable, int k)
{
	int x = 0;

	while(lexTable[x++]->name != NULL)
	{
		printf("%s\t%d\n", lexTable[x]->name, lexTable[x]->type);
	}
}
