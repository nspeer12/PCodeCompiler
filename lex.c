#include <stdio.h>

// REad a source porgram written in PL/0,
// identify errors
// output source program, lexeme table, and list of lexemes

int main(int argc, char *argv[])
{
	// echo program
	// File IO
	FILE * fp;

	char buffer[32];
	if (argc > 0)
	{
		fp = fopen(argv[1], "r");
	}

	int i = 0;
	char tmp;
	while(!feof(fp))
	{
		tmp = fgetc(fp);
		if (tmp == ',')
		{
			printf(" comma ");
			i = 0;
		}
		else if (tmp == ' ')
		{
			printf(" space ");
			i = 0;
		}
		else if (tmp == ';')
		{
			printf(" semicolon ");
		}
		else if (tmp == '\n')
		{
			printf(" newline\n");
			i = 0;
		}
		else
		{
			printf("%c", tmp);
			buffer[i] = tmp;
		}
		i++;
	}

	return 0;
}
