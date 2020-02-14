#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Read a source porgram written in PL/0,
// identify errors
// output source program, lexeme table, and list of lexemes

int echoFile(char * filename);

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


	int len;
	FILE * fp;
	if (argc > 0)
	{
		len = echoFile(argv[1]);
		fp = fopen(argv[1], "r");
	}



	lexeme ** lexTable = malloc(sizeof(lexeme *) * len);

	for (int i = 0; i < len; i++)
	{
		lexTable[i] = malloc(sizeof(lexeme));
		lexTable[i]->name = malloc(sizeof(char) * 12);
	}

	int k = 0; // keep track of lexeme table index
	int i = 0; // buffer index
	char tmp = fgetc(fp);
	// buffer of size 12 because max variable length is 11
	char * buffer = malloc(sizeof(char) * 12);

	while(!feof(fp))
	{

		if (tmp == ',')
		{
			insertTable(lexTable, buffer, k++);
			for (int i = 0; i < 12; i ++)
				buffer[i] = '\0';
			insertTable(lexTable, ",", k++);
			i = 0;
		}
		else if (tmp == ';')
		{
			insertTable(lexTable, buffer, k++);
			for (int i = 0; i < 12; i ++)
				buffer[i] = '\0';

			insertTable(lexTable, ";", k++);
			i = 0;
		}
		else if (tmp == '\n' || tmp == '\t' || tmp == ' ')
		{
			insertTable(lexTable, buffer, k++);
			for (int i = 0; i < 12; i ++)
				buffer[i] = '\0';
			i = 0;
		}
		else
		{
			buffer[i] = tmp;
			buffer[++i] = '\0';
		}
		tmp = fgetc(fp);

	}

	for(int x = 0; x < k; x ++)
	{
		printf("%s\n", lexTable[x]->name);
	}

	// printLexTable(lexTable, 5);

	return 0;
}


int echoFile(char * filename)
{
	int len = 0;

	FILE * fp = fopen(filename, "r");

	while(!feof(fp))
	{

		printf("%c", fgetc(fp));
		len++;
	}

	fclose(fp);
	return len;
}

void insertTable(lexeme ** lexTable, char * buffer, int k)
{
	if (buffer[0] != '\0')
	{
		strcpy(lexTable[k]->name, buffer);
		//printf("%s\n", lexTable[k]->name);
		// temporarily use this for now
		lexTable[k]->type = k;
	}

	return;
}
