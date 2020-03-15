// Written by Stephen Speer
// Written by Danish Siddiqui

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct lexeme
{
		char name[11];
		int type;

}lexeme;

lexeme masterArray[10000];

int getNumChars(FILE *fp);
void populateCharArray(char * charArray, int numOfChars, char * filename);
void cleanInput(int numOfChars, char charArray[]);
void evaluateTokens(char charArray[]);
int isALetter(char c);
int isANumber(char c);
int isReservedWord(char temp[]);
void identify(char temp[], int ssym[], int *lexemesLength);
void printLexemeList(int *lexemesLength);
int isSpecialSymbol(char c);
int echoFile(char * filename);
void writeLexemeList(int *lexemesLength);

typedef enum
{
	nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6, slashsym = 7, oddsym = 8,
	eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
	commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23,
	thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
	readsym = 32, elsesym = 33

}token_type;


char *word[] =
{
	"null", "begin", "call", "const", "do", "else", "end", "if",
	"odd", "procedure", "read", "then", "var", "while", "write"
};

int wsym [] = {
	nulsym, beginsym, callsym, constsym, dosym, elsesym, endsym,
	ifsym, oddsym, procsym, readsym, thensym, varsym, whilesym, writesym
};

char symbols[] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':'};

int main(int argc, char *argv[])
{
		FILE *fp;
		int i =0;
		int numOfChars;
		char * charArray;
		if (argc > 0)
		{
			numOfChars = echoFile(argv[1]);
			charArray = malloc(sizeof(char) * (numOfChars  + 2));
			populateCharArray(charArray, numOfChars, argv[1]);

		}
		else
		{
			printf("Error! Please pass a valid filename through command line argument\n");
		}

		// clears up all comments, \n and tabs.
		cleanInput(numOfChars, charArray);
		evaluateTokens(charArray);
}

void evaluateTokens(char charArray[])
{
	printf("Lexeme Table: \n");
	int lexemesLength = 0 ;
	int ssym[256];
	ssym['+'] = plussym; ssym['-'] = minussym; ssym['*'] = multsym;
	ssym['/'] = slashsym; ssym['('] = lparentsym; ssym[')'] = rparentsym;
	ssym['='] = eqlsym; ssym[','] = commasym; ssym['.'] = periodsym;
	ssym['#'] = neqsym; ssym['<'] = lessym; ssym['>'] = gtrsym;
	ssym['$'] = leqsym; ssym['%'] = geqsym; ssym[';'] = semicolonsym;

	printf("lexeme\ttoken type\n");

	int i = 0, z =0;
	char temp[15];
	for(z = 0; z < 15; z++)
			temp[z] = '\0';

	for( i = 0; i < strlen(charArray); i++)
	{
		char c = charArray[i];
		// get to a letter.
		if(c == ' ')
		{
			if(strlen(temp)>0)
			{
				identify(temp,ssym,&lexemesLength);
				lexemesLength++;
			}

		}
		else if(isSpecialSymbol(c))
		{
			if(strlen(temp) == 0)
			{
				if(c==':' && charArray[i+1]=='=')
				{
					temp[strlen(temp)] = c;
					temp[strlen(temp)] = charArray[++i];
					identify(temp,ssym,&lexemesLength);
					lexemesLength++;
				}
				else
				{
					temp[strlen(temp)] = c;
					identify(temp,ssym,&lexemesLength);
					lexemesLength++;
				}

			}
			else
			{
				identify(temp,ssym,&lexemesLength);
				lexemesLength++;
				i--;
			}
		}
		else
		{
			temp[strlen(temp)] = c;
		}
	}

	printLexemeList(&lexemesLength);
	writeLexemeList(&lexemesLength);
}
void cleanInput(int numOfChars, char charArray[])
{
	 int i;

	 // Removes tabs.
	 for(i = 0; i < numOfChars; i++)
	 {
		 if(charArray[i]=='\t' || charArray[i]=='\n')
		 {
			 charArray[i] = ' ';
		 }
	 }

	 // Removes comments.
	 for(i = 0; i < numOfChars; i++)
	 {
		 // check for a comment sequence while iterating through char array.
		 if(charArray[i] == '/' && i+1 < numOfChars && charArray[i+1] == '*')
		 {
			 charArray[i] = ' ';
			 charArray[i+1] = ' ';
			 i+=2;

			 while(i < numOfChars)
			 {
					if(charArray[i] == '*')
					{
						 if(charArray[i+1] == '/')
						 {
								charArray[i++] = ' ';
								charArray[i++] = ' ';
								break;
						 }
						 else
						 {
								charArray[i++] = ' ';
								charArray[i++] = ' ';
						 }
					}
					else
					{
						 charArray[i++] = ' ';
					}
			 }
		 }
	 }
}

void populateCharArray(char * charArray, int numOfChars, char * filename)
{
	char c;
	int i = 0;
	FILE * fp = fopen(filename, "r");

	while(fscanf(fp,"%c",&c) != EOF)
		charArray[i++] = c;

	fclose(fp);
}

int getNumChars(FILE *fp)
{
	char c;
	int i = 0;

	while(fscanf(fp,"%c",&c) != EOF)
		i++;

	return i;
}

int isANumber(char c)
{
		int value = c - '0';
		if(value >=0 && value < 10)
				return 1;
		return 0;
}

int isALetter(char c)
{
		int value = c - 'a';
		if(value >= 0 && value < 26)
				return 1;

		int value2 = c - 'A';
		if(value2 >= 0 && value2 < 26)
				return 1;

		return 0;
}
int isReservedWord(char temp[])
{
		int i;
		for(i = 0; i < 15; i++)
		{
				if(strcmp(temp, word[i]) == 0)
				{
						return wsym[i];
				}
		}
		return 0;
}

void identify(char temp[], int ssym[], int *lexemesLength)
{
	int z = 0;
	lexeme current;

	if(strlen(temp) > 0)
	{
		printf("%s\t",temp);

		// perform checks to see what temp qualifies as.
		// Does it qualify as a special word?
		if(isReservedWord(temp) > 0)
		{
			printf("%d\n",isReservedWord(temp));

			current.type = isReservedWord(temp);
			strcpy(current.name,temp);
			masterArray[*lexemesLength] = current;

		}
		else if(isSpecialSymbol(temp[0]) > 0)
		{
			if(strlen(temp) > 1)
			{
				if(temp[1]=='=')
				{
					printf("%d\n",becomessym);

					current.type = becomessym;
					strcpy(current.name,temp);
					masterArray[*lexemesLength] = current;
				}
			}
			else
			{
				printf("%d\n",ssym[temp[0]]);
				current.type = ssym[temp[0]];
				strcpy(current.name,temp);
				masterArray[*lexemesLength] = current;
			}
		}
		else if(isALetter(temp[0]))
		{
			if(strlen(temp)>11)
			{
				// Throw ERROR # 3.
				printf("ERROR #3: Name longer than 11 chars.\n");
			}
			else
			{
				printf("%d\n",identsym);
				current.type = identsym;
				strcpy(current.name,temp);
				masterArray[*lexemesLength] = current;
			}

		}
		else if(isANumber(temp[0]))
		{
			int falseCount = 0;
			int k;

			for(k =0; k<strlen(temp); k++)
				if(!isANumber(temp[k]))
					falseCount++;

			if(falseCount>0)
			{
				// Throw Error #1
				printf("ERROR #1: Variable Starts with a Number!\n");
			}
			if(strlen(temp)>5)
			{
				// Throw ERROR # 2.
				printf("ERROR #2: Number longer than 5 digits!\n");
			}
			else
			{
				printf("%d\n", numbersym);
				current.type = numbersym;
				strcpy(current.name,temp);
				masterArray[*lexemesLength] = current;
			}

		}
		else
		{
			// Throw Error 4
			printf("ERROR #4 : Invalid Symbol.\n");
		}

		for(z = 0; z < 15; z++)
				temp[z] = '\0';
	}
}

int isSpecialSymbol(char c)
{
		int i;
		for(i=0; i < 13; i++)
		{
				if(c == symbols[i])
						return 1;
		}
		return 0;
}

void printLexemeList(int *lexemesLength)
{
	printf("\nLexeme List: \n");

	for(int i = 0; i< *lexemesLength; i++)
	{
		printf("%d ",masterArray[i].type);
		if(masterArray[i].type == numbersym || masterArray[i].type == identsym)
			printf("%s ",masterArray[i].name);
	}
	printf("\n");
}

void writeLexemeList(int *lexemesLength)
{
	FILE * typeFile = fopen("tmp/lex.type.output", "w");
	FILE * nameFile = fopen("tmp/lex.name.output", "w");

	for(int i = 0; i< *lexemesLength; i++)
	{
		fprintf(typeFile, "%d ",masterArray[i].type);
		fprintf(nameFile, "%s ",masterArray[i].name);

		/*
		if(masterArray[i].type == numbersym || masterArray[i].type == identsym)
			fprintf(fp, "%s ",masterArray[i].name);
		*/
	}
	printf("\n");
}

int echoFile(char * filename)
{
	int len = 0;

	FILE * fp = fopen(filename, "r");
	int tmp;

	printf("Source Program:\n");

	while((tmp = fgetc(fp)) != EOF)
	{
		printf("%c", tmp);
		len++;
	}

	printf("\n");
	fclose(fp);
	return len;
}
