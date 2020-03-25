// Written by Stephen Speer
// Written by Danish Siddiqui

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct lexeme
{
		char name[15];
		int type;
		char inputValue[15];
}lexeme;

lexeme masterArray[10000];

int getNumChars(FILE *fp);
void populateCharArray(char * charArray, int numOfChars, char * filename);
void cleanInput(int numOfChars, char charArray[]);
void evaluateTokens(char charArray[], int print);
int isALetter(char c);
int isANumber(char c);
int isReservedWord(char temp[]);
void identify(char temp[], int ssym[], int *lexemesLength, int print);
void printLexemeList(int *lexemesLength);
int isSpecialSymbol(char c);
int echoFile(char * filename, int print);
char * getReservedWordName(char temp[]);
char * getReservedSymName(char t);
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
		int print = 0;
		char * filename = malloc(sizeof(char) * 64);

		if (argc >= 2)
		{

			if (argc > 3)
			{
				if (strcmp(argv[1], "-l") == 0)
				{
					print = 1;

					strcpy(filename, argv[2]);

					// printf("filename: %s", filename);
				}
				else
				{
					strcpy(filename, argv[1]);
				}
			}
			else
			{

				strcpy(filename, argv[1]);

			}
		}

		numOfChars = echoFile(filename, print);
		charArray = malloc(sizeof(char) * (numOfChars  + 2));
		populateCharArray(charArray, numOfChars, filename);

		// clears up all comments, \n and tabs.
		cleanInput(numOfChars, charArray);
		evaluateTokens(charArray, print);
}

char * getReservedSymName(char t)
{
	char *result = NULL;
	result = malloc(sizeof(char)*12);
/*
	ssym['+'] = plussym; ssym['-'] = minussym; ssym['*'] = multsym;
	ssym['/'] = slashsym; ssym['('] = lparentsym; ssym[')'] = rparentsym;
	ssym['='] = eqlsym; ssym[','] = commasym; ssym['.'] = periodsym;
	ssym['#'] = neqsym; ssym['<'] = lessym; ssym['>'] = gtrsym;
	ssym['$'] = leqsym; ssym['%'] = geqsym; ssym[';'] = semicolonsym;
*/

	if(t=='+')
		strcpy(result,"plussym");

	if(t=='/')
		strcpy(result,"slashsym");

	if(t=='=')
		strcpy(result,"eqlsym");

	if(t=='#')
		strcpy(result,"neqsym");

	if(t=='$')
		strcpy(result,"leqsym");

	if(t=='-')
		strcpy(result,"minussym");

	if(t=='(')
		strcpy(result,"lparentsym");

	if(t==',')
		strcpy(result,"commasym");

	if(t=='<')
		strcpy(result,"lessym");

	if(t=='%')
		strcpy(result,"geqsym");

	if(t=='*')
		strcpy(result,"multsym");

	if(t==')')
		strcpy(result,"rparentsym");

	if(t=='.')
	 	strcpy(result,"periodsym");

	if(t=='>')
		strcpy(result,"gtrsym");

	if(t==';')
		strcpy(result,"semicolonsym");

	return result;
}

char * getReservedWordName(char temp[])
{
	/*
	nulsym, beginsym, callsym, constsym, dosym, elsesym, endsym,
	ifsym, oddsym, procsym, readsym, thensym, varsym, whilesym, writesym
	*/
	int i;
	char *result = NULL;
	result = malloc(sizeof(char)*12);

	for(i = 0; i < 15; i++)
	{
			if(strcmp(temp, word[i]) == 0)
			{
					if(i==0)
						strcpy(result,"nulsym");

					if(i==1)
						strcpy(result,"beginsym");

					if(i==2)
						strcpy(result,"callsym");

					if(i==3)
						strcpy(result,"constsym");

					if(i==4)
						strcpy(result,"dosym");

					if(i==5)
						strcpy(result,"elsesym");

					if(i==6)
						strcpy(result,"endsym");

					if(i==7)
						strcpy(result,"ifsym");

					if(i==8)
						strcpy(result,"oddsym");

					if(i==9)
						strcpy(result,"procsym");

					if(i==10)
						strcpy(result,"readsym");

					if(i==11)
						strcpy(result,"thensym");

					if(i==12)
						strcpy(result,"varsym");

					if(i==13)
						strcpy(result,"whilesym");

					if(i==14)
						strcpy(result,"writesym");

			}
	}
	return result;
}

void evaluateTokens(char charArray[], int print)
{
	if (print)
		printf("Lexeme Table: \n");

	int lexemesLength = 0 ;
	int ssym[256];
	ssym['+'] = plussym; ssym['-'] = minussym; ssym['*'] = multsym;
	ssym['/'] = slashsym; ssym['('] = lparentsym; ssym[')'] = rparentsym;
	ssym['='] = eqlsym; ssym[','] = commasym; ssym['.'] = periodsym;
	ssym['#'] = neqsym; ssym['<'] = lessym; ssym['>'] = gtrsym;
	ssym['$'] = leqsym; ssym['%'] = geqsym; ssym[';'] = semicolonsym;

	if (print)
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
				identify(temp,ssym,&lexemesLength, print);
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
					identify(temp,ssym, &lexemesLength, print);
					lexemesLength++;
				}
				else
				{
					temp[strlen(temp)] = c;
					identify(temp,ssym, &lexemesLength, print);
					lexemesLength++;
				}

			}
			else
			{
				identify(temp, ssym, &lexemesLength, print);
				lexemesLength++;
				i--;
			}
		}
		else
		{
			temp[strlen(temp)] = c;
		}
	}

	if (print)
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

void identify(char temp[], int ssym[], int *lexemesLength, int print)
{
	int z = 0;
	lexeme current;

	if(strlen(temp) > 0)
	{
		if (print)
			printf("%s\t",temp);

		// perform checks to see what temp qualifies as.
		// Does it qualify as a special word?
		if(isReservedWord(temp) > 0)
		{
			if (print)
				printf("%d\n",isReservedWord(temp));

			current.type = isReservedWord(temp);
			strcpy(current.name,getReservedWordName(temp));
			strcpy(current.inputValue,temp);
			masterArray[*lexemesLength] = current;

		}
		else if(isSpecialSymbol(temp[0]) > 0)
		{
			if(strlen(temp) > 1)
			{
				if(temp[1]=='=')
				{
					if (print)
						printf("%d\n",becomessym);

					current.type = becomessym;
					strcpy(current.name,"becomessym");
					strcpy(current.inputValue,temp);
					masterArray[*lexemesLength] = current;
				}
			}
			else
			{
				if (print)
					printf("%d\n",ssym[temp[0]]);

				current.type = ssym[temp[0]];
				strcpy(current.inputValue,temp);
				strcpy(current.name,getReservedSymName(temp[0]));
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
				if (print)
					printf("%d\n",identsym);

				current.type = identsym;
				strcpy(current.inputValue,temp);
				strcpy(current.name,"identsym");
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
				if (print)
					printf("%d\n", numbersym);

				current.type = numbersym;
				strcpy(current.name,"numbersym");
				strcpy(current.inputValue,temp);
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

int echoFile(char * filename, int print)
{
	int len = 0;

	FILE * fp = fopen(filename, "r");
	int tmp;

	if (print)
		printf("Source Program:\n");

	while((tmp = fgetc(fp)) != EOF)
	{
		if(print)
			printf("%c", tmp);
		len++;
	}

	if (print)
		printf("\n");

	fclose(fp);
	return len;
}

void writeLexemeList(int *lexemesLength)
{
	char *errors = NULL;
	errors = malloc(sizeof(char)*100000);

	//FILE * typeFile = fopen("tmp/lex.type.output", "w");
	//FILE * nameFile = fopen("tmp/lex.name.output", "w");
	//FILE * valueFile = fopen("tmp/lex.value.output", "w")

	FILE * lexOutput = fopen("tmp/lex.output", "w");

	for(int i = 0; i < *lexemesLength; i++)
	{
		fprintf(lexOutput, "%s\t",masterArray[i].name);
		fprintf(lexOutput, "%d\t",masterArray[i].type);
		fprintf(lexOutput, "%s", masterArray[i].inputValue);
		fprintf(lexOutput, "\n");
	}
}

void printLexemeList(int *lexemesLength)
{
	char *errors = NULL;
	errors = malloc(sizeof(char)*100000);

	printf("\nLexeme List: \n");

	for(int i = 0; i < *lexemesLength; i++)
	{
		printf("%d ",masterArray[i].type);
		if(masterArray[i].type == numbersym || masterArray[i].type == identsym)
		{
			printf("%s ",masterArray[i].inputValue);
		}

	}

	printf("\n");
	printf("\n");

	for(int i = 0; i < *lexemesLength; i++)
	{
		printf("%s ",masterArray[i].name);
		if(masterArray[i].type == numbersym || masterArray[i].type == identsym)
		{
			printf("%s ",masterArray[i].inputValue);
		}

	}
	printf("\n");
}
