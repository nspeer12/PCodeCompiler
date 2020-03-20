#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "lex.c"

typedef struct symbol
{
    int kind;       // const = 1, var = 2, proc = 3
    char name[12];  // name (up to 11 chars)
    int val;        // initial value
    int level;      // Lexicographical level
    int addr;       // Mem address
	 struct symbol * next; // for linked list
} symbol;

typedef struct token
{
    int type;
    char * name;
    char * inputValue;
    struct token * next;

} token;

void parser(char * nameFile, char * typeFile);
void insertTail(token * head, char * name, int type);
void printList(token * head);
int getFileLen(char * filename);
char * fileToArr(char * filename);
void parser(char * nameFile, char * typeFile);
token * getTokenList(char * nameFile, char * typeFile);
token * fetch(token * tok);

// parser functions
token * block(token * tok);
token * program(token * tok);
token * statement(token * tok);
token * expression(token * tok);
token * condition(token * tok);
token * factor(token * tok);
token * term(token * tok);
token * linkListify(int *lexemesLength);

int main(int argc, char ** argv)
{

    int MAX_SYMBOL_TABLE_SIZE = 256; // Need to check this
    symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

    // LEX MAIN BEGIN.

    FILE *fp;
    int i =0;
    int numOfChars;
    char * charArray;

    int print = 0;

    if (argc > 0)
    {
      if (argc == 3)
      {
        if (strcmp(argv[2], "-l") == 0)
          print = 1;
      }

      numOfChars = echoFile(argv[1], print);
      charArray = malloc(sizeof(char) * (numOfChars  + 2));
      populateCharArray(charArray, numOfChars, argv[1]);
    }
    else
    {
      printf("Error! Please pass a valid filename through command line argument\n");
    }

    // clears up all comments, \n and tabs.
    cleanInput(numOfChars, charArray);
    evaluateTokens(charArray, print);

    // LEX MAIN END.

    /* Parser
        - read the output of the Lex scanner (hw2)
        - parse the tokens
        - output if the program is semantically correct
        - print out where errors exist
        - fill out symbol table and print it
    */

   /* Intermediate Code Generation
        - takes the symbol table and produces the assembly for the VM
        - execute the VM
   */

    parser("tmp/lex.name.output", "tmp/lex.type.output");

    return 0;
}




void parser(char * nameFile, char * typeFile)
{
	printf("*** PARSER ***\n");
   // dynamically allocated based on how many tokens there are

	// get linked list with tokens
	token * head = linkListify(&masterLength);
	printList(head);
	printf("\n");

	// create symbol table
		// symbol table is a linked list with head insertions
		// I don't give a fuck about runtime here

	symbol * sym;

	// get the first token from the list
	token * tok = head;
	program(tok);
   // psuedocode implementation
    return;
}

token * program(token * tok)
{
	printf("\t***\tPROGRAM\t***\n");
	tok = fetch(tok);
	if (tok == NULL)
	{
		return NULL;
	}
	else
	{
		tok = block(tok);
		if (strcmp(tok->name, "periodsym") != 0)
			printf("I missed my period... I think I might be pregant\n");
		printf("%s", tok->name);
	}
}
token * linkListify(int *lexemesLength)
{
  token * head = malloc(sizeof(token));

  head->name = malloc(sizeof(char)*16);
  head->inputValue = malloc(sizeof(char)*16);
  head->type = -1;

  strcpy(head->name, "head");
  strcpy(head->inputValue, "head");
  head->type = 0;
  head->next = NULL;

  token *temp = head;

  for(int i = 0; i<*lexemesLength; i++)
  {
    // copy over all the information into this linked list.
    token *new = malloc(sizeof(token));

    new->name = malloc(sizeof(char)*16);
    new->inputValue = malloc(sizeof(char)*16);

    new->type = masterArray[i].type;
    strcpy(new->inputValue,masterArray[i].inputValue);
    strcpy(new->name, masterArray[i].name);

    temp->next = new;
    temp = temp->next;
  }

  return head;
}
token * block(token * tok)
{

	printf("\t***\tBLOCK\t***", tok->name);

	if (strcmp(tok->name, "constsym") == 0)
	{
		tok = fetch(tok);
		while(strcmp(tok->name, "commasym") == 0)
		{
			if (strcmp(tok->name,"identsym") != 0)
			{
				printf("identsym error in block/constsym");
				return NULL;
			}

			tok = fetch(tok);

			if (strcmp(tok->name,"eqsym") != 0)
			{
				printf("eqsym error in block/constsym\n");
				return NULL;
			}

			tok = fetch(tok);

			if (strcmp(tok->name, "numbersym") != 0)
			{
				// insert into symbol table??
				printf("numbersym error in block/constsym\n");
				return NULL;
			}

			tok = fetch(tok);
		}
	}

	if (strcmp(tok->name, "insym") == 0)
	{
		while(strcmp(tok->name, "commasym") == 0)
		{
			tok = fetch(tok);
			if (strcmp(tok->name, "identsym") != 0)
			{
				printf("identsym error in block/insym\n");
				return NULL;
			}

			tok = fetch(tok);
		}

		if (strcmp(tok->name, "semicolonsym") != 0)
		{
			printf("semicolosym error in block/insym\n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}
	}

	while(strcmp(tok->name,"procsym") == 0)
	{
		tok = fetch(tok);
		if (strcmp(tok->name, "identsym") != 0)
		{
			printf("identsym error in block/procsym\n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}

		if (strcmp(tok->name, "semicolonsym") != 0)
		{
			printf("semicolonsym error in block/procsym\n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}
	}

	tok = statement(tok);
	return tok;


}

token * statement(token * tok)
{

	printf("\t***\tSTATEMENT\t***\n");

	if (strcmp(tok->name, "identsym") == 0)
	{
		tok = fetch(tok);
		if (strcmp(tok->name, "becomessym") != 0)
		{
			printf("becomessym error in statement/indentsym: %s\n", tok->name);
			return NULL;
		}
		tok = fetch(tok);
		tok = expression(tok);
	}
	else if (strcmp(tok->name, "callsym") == 0)
	{
		tok = fetch(tok);
		if (strcmp(tok->name, "identsym") != 0)
		{
			printf("identsym err in statement/callsym\n");
			return NULL;
			tok = fetch(tok);
		}
	}
	else if (strcmp(tok->name, "beginsym") == 0)
	{
		tok = fetch(tok);
		tok = statement(tok);

		while( strcmp(tok->name, "semicolonsym") == 0)
		{
			tok = fetch(tok);
			tok = statement(tok);
		}

		if (strcmp(tok->name, "endsym") != 0)
		{
			printf("endsym err in statement/beginsym: %s\n", tok->name);
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}
	}
	else if (strcmp(tok->name, "ifsym") == 0)
	{
		tok = fetch(tok);
		tok = condition(tok);
		if ( strcmp(tok->name, "thensym") != 0)
		{
			printf("thensym error in statament/ifsym\n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
			tok = statement(tok);
		}
	}
	else if (strcmp(tok->name, "whilesym") == 0)
	{
		tok = fetch(tok);
		tok = condition(tok);

		if (strcmp(tok->name, "dosym") != 0)
		{
			printf("dosym error in statement/whilesym\n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
			tok = statement(tok);
		}
	}

	return tok;
}

token * condition(token * tok)
{

	printf("\t***\tCONDITION\t***\n");

	// this may be wrong, per his psuedocode
	if (strcmp(tok->name, "oddsym") == 0)
	{
		tok = fetch(tok);
		tok = expression(tok);
	}
	else
	{
		tok = expression(tok);
		// TODO: check if it's a relation symbol like == or whatever
		if (strcmp(tok->name, "relation") != 0)
		{
			printf("relation error in condition/else \n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
			tok = expression(tok);
		}
	}

	return tok;
}

token * expression(token * tok)
{

	printf("\t***\tEXPRESSION\t***\n");

	if (strcmp(tok->name, "plussym") == 0 || strcmp(tok->name, "minussym") == 0)
	{
		tok = term(tok);

		while(strcmp(tok->name, "plussym") == 0 || strcmp(tok->name, "minussym") == 0)
		{
			tok = fetch(tok);
			tok = term(tok);
		}
	}

	return tok;
}

token * term(token * tok)
{

	printf("\t***\tTERM\t***\n");

	tok = factor(tok);

	while(strcmp(tok->name, "multsym") == 0 || strcmp(tok->name, "slashym") == 0)
	{
		tok = fetch(tok);
		tok = factor(tok);
	}
}


token * factor(token * tok)
{

	printf("\t***\tFACTOR\t***\n");

	if (strcmp(tok->name, "identsym") == 0)
	{
		tok = fetch(tok);
	}
	// TODO identify as a number
	else if (strcmp(tok->name, "numbersym") == 0)
	{
		tok = fetch(tok);
	}
	else if (strcmp(tok->name, "lparentsym") == 0)
	{
		tok = fetch(tok);
		tok = expression(tok);
		if (strcmp(tok->name, "rparentsym") != 0)
		{
			printf("missing ) in factor/(\n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}
	}
	else
	{
		printf("error in factor\n");
	}

	return tok;
}

token * fetch(token * tok)
{
	if (tok == NULL)
	{
		printf("Null token Error!\n");
		return NULL;
	}
	else if (tok->next == NULL)
	{
		printf("Null next token error!\n");
		return NULL;
	}
	else
	{
		printf("- %s ", tok->name);
		tok = tok->next;
		printf("-> %s\n", tok->name);
		return tok;
	}
}


symbol * insertSym(symbol * sym, int kind, char * name, int val, int level, int addr)
{
		symbol * tmp = malloc(sizeof(symbol));
		tmp->kind = kind;
		strcpy(tmp->name, name);
		tmp->val = val;
		tmp->level = level;
		tmp->addr = -69;
		tmp->next = sym;
		sym = tmp;

		return sym;

}

token * getTokenList(char * nameFile, char * typeFile)
{
		// THIS FUNCTION IS A BITCH
		// AVOID TINKERING WITH IT

		// create a new linked list to store tokens
		token * head = malloc(sizeof(token));
		head->name = malloc(sizeof(char) * 13);
		head->type = -1;

		strcpy(head->name, "head");
		head->type = 0;
		head->next = NULL;

		char * nameArr = fileToArr(nameFile);
		char * namePtr = nameArr;

		char * typeArr = fileToArr(typeFile);

		char * typePtr = typeArr;
		char * nameTok, * typeTok;

		// iterate through tokens and build linked list


		while( (nameTok = strtok_r(namePtr, " ", &namePtr)) && (typeTok = strtok_r(typePtr, " ", &typePtr)) )
		{

			char * tmpStr = malloc(sizeof(char) * 13);
			strcpy(tmpStr, nameTok);

			int tmpType = atoi(typeTok);


			// insert into tail of linked list
			token * tmp = head;

			while(tmp->next != NULL)
				tmp = tmp->next;

			tmp->next = malloc(sizeof(token));
			tmp->next->name = tmpStr;
			tmp->next->type = tmpType;
			tmp->next->next = NULL;

			//printf("%s\t%d\n", tmp->next->name, tmp->next->type);
		}

		printList(head);



	return head;
}

void printList(token * head)
{
	token * tmp = head->next;

	// print list
	while(tmp != NULL)
	{
		printf("%s\t%d\n", tmp->name, tmp->type);
		tmp = tmp->next;
	}

   printf("END OF LIST\n");
}

int getFileLen(char * filename)
{
	int fileLen = 0;
	FILE * fp = fopen(filename, "r");

	while(!feof(fp))
	{
		fgetc(fp);
		fileLen++;
	}

	fclose(fp);
	return fileLen;
}

char * fileToArr(char * filename)
{
	char * arr = malloc(sizeof(char) * getFileLen(filename));
	FILE * fp = fopen(filename, "r");
	char tmp;

	int i = 0;

	while(!feof(fp))
	{
		tmp = fgetc(fp);
		if (tmp == EOF)
		break;
		else
		{
			arr[i++] = tmp;
		}
	}

	return arr;
}
