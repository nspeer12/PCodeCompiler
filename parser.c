#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

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
    char * value;
    struct token * next;

} token;

typedef enum
{
	nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6, slashsym = 7, oddsym = 8,
	eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
	commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23,
	thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
	readsym = 32, elsesym = 33

}token_type;

void insertTail(token * head, char * name, int type);
void printList(token * head);
int getFileLen(char * filename);
char * fileToArr(char * filename);
token * linkListify(int *lexemesLength);
token * getTokenList(char * lexFile);
token * fetch(token * tok);

// parser functions
void parser(char * nameFile, char * typeFile);
token * block(token * tok);
token * program(token * tok);
token * statement(token * tok);
token * expression(token * tok);
token * condition(token * tok);
token * factor(token * tok);
token * term(token * tok);


int main(int argc, char ** argv)
{

    //int MAX_SYMBOL_TABLE_SIZE = 256; // Need to check this
   // symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

    // LEX MAIN BEGIN.

   // FILE *fp;
	// char * filename;
   // int i =0;
   // int numOfChars;
   // char * charArray;

	printf("here");
	token * head = getTokenList("tmp/lex.output");
   //parser("tmp/lex.name.output", "tmp/lex.type.output");

    return 0;
}


void parser(char * nameFile, char * typeFile)
{
	printf("*** PARSER ***\n");
   // dynamically allocated based on how many tokens there are

	// get linked list with tokens
	// Fucking shit
	//token * head = linkListify(&masterLength);
	//printList(head);
	printf("\n");

	// create symbol table
		// symbol table is a linked list with head insertions
		// I don't give a fuck about runtime here

	//symbol * sym;

	// get the first token from the list
	//token * tok = head;
	//program(tok);
   // psuedocode implementation
    return;
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

token * getTokenList(char * lexFile)
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
		head->value = NULL;

		//char * lexArr = fileToArr(lexFile);

		char buff[80];

		FILE * fp = fopen(lexFile, "r");

		// iterate through tokens and build linked list

		while(fgets(buff, sizeof(buff), fp) != NULL)
		{
			// go to tail of linked list
			token * tmp = head;

			while(tmp->next != NULL)
				tmp = tmp->next;

			tmp->next = malloc(sizeof(token));
			tmp->next->next = NULL;
			tmp = tmp->next;
			tmp->name = malloc(sizeof(char) * 12);
			tmp->value = malloc(sizeof(char) * 12);

			// scan and insert into tail of linked list
			sscanf(buff, "%s%d%s", tmp->name, &tmp->type, tmp->value);
		}

		printList(head);

	return head;
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
		if (tok->type != periodsym)
			printf("Missing period in program\n");
		printf("%s", tok->name);
	}

  return tok;
}

token * block(token * tok)
{
  // there was an error with this line so I changed it: printf("\t***\tBLOCK\t***", tok->name);
	printf("\t***\tBLOCK\t***");

	if (tok->type == constsym)
	{
		tok = fetch(tok);
		while(tok->type == commasym)
		{
			if (tok->type != identsym)
			{
				printf("identsym error in block/constsym");
				return NULL;
			}

			tok = fetch(tok);

			if (tok->type != eqlsym)
			{
				printf("eqsym error in block/constsym\n");
				return NULL;
			}

			tok = fetch(tok);

			if (tok->type != numbersym)
			{
				// insert into symbol table??
				printf("numbersym error in block/constsym\n");
				return NULL;
			}

			tok = fetch(tok);
		}
	}

	// TODO: intsym is not defined or specified
	/*
	if (tok->type == intsym)
	{
		while(tok->type == commasym)
		{
			tok = fetch(tok);
			if (tok->type != identsym)
			{
				printf("identsym error in block/insym\n");
				return NULL;
			}

			tok = fetch(tok);
		}

		if (tok->type != semicolonsym)
		{
			printf("semicolosym error in block/insym\n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}
	}
	*/
	while(tok->type == procsym)
	{
		tok = fetch(tok);
		if (tok->type != identsym)
		{
			printf("identsym error in block/procsym\n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}

		if (tok->type != semicolonsym)
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

	if (tok->type == constsym)
	{
		tok = fetch(tok);
		if (tok->type != becomessym)
		{
			printf("becomessym error in statement/indentsym: %s\n", tok->name);
			return NULL;
		}
		tok = fetch(tok);
		tok = expression(tok);
	}
	else if (tok->type == callsym)
	{
		tok = fetch(tok);
		if (tok->type != identsym)
		{
			printf("identsym err in statement/callsym\n");
			return NULL;
			tok = fetch(tok);
		}
	}
	else if (tok->type == beginsym)
	{
		tok = fetch(tok);
		tok = statement(tok);

		while( tok->type == semicolonsym)
		{
			tok = fetch(tok);
			tok = statement(tok);
		}

		if (tok->type != endsym)
		{
			printf("endsym err in statement/beginsym: %s\n", tok->name);
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}
	}
	else if (tok->type == ifsym)
	{
		tok = fetch(tok);
		tok = condition(tok);
		if (tok->type != thensym)
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
	else if (tok->type == whilesym)
	{
		tok = fetch(tok);
		tok = condition(tok);

		if (tok->type != dosym)
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
	if (tok->type == oddsym)
	{
		tok = fetch(tok);
		tok = expression(tok);
	}
	else
	{
		tok = expression(tok);
		// TODO: check if it's a relation symbol like == or whatever
		// I deleted relation from here. we need to include it
		if (tok->type != -1)
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

	if (tok->type == plussym || tok->type == minussym)
	{
		tok = term(tok);

		while(tok->type == plussym || tok->type == minussym)
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

	while(tok->type == multsym || tok->type == slashsym)
	{
		tok = fetch(tok);
		tok = factor(tok);
	}
}

token * factor(token * tok)
{

	printf("\t***\tFACTOR\t***\n");

	if (tok->type == identsym)
	{
		tok = fetch(tok);
	}
	// TODO identify as a number
	else if (tok->type == numbersym)
	{
		tok = fetch(tok);
	}
	else if (tok->type == lparentsym)
	{
		tok = fetch(tok);
		tok = expression(tok);
		if (tok->type != rparentsym)
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

void printList(token * head)
{
	token * tmp = head->next;

	// print list
	while(tmp != NULL)
	{
		printf("%s\t%d\t%s\n", tmp->name, tmp->type, tmp->value);
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
