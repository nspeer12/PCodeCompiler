#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "token.h"

typedef struct symbol
{
    int kind;       // const = 1, var = 2, proc = 3
    char name[12];  // name (up to 11 chars)
    int val;        // initial value
    int level;      // Lexicographical level
    int addr;       // Mem address
	 struct symbol * next; // for linked list
} symbol;

token * fetch(token * tok);

// parser functions
void parser(token * tok);
token * block(token * tok);
token * program(token * tok);
token * statement(token * tok);
token * expression(token * tok);
token * condition(token * tok);
token * factor(token * tok);
token * term(token * tok);
int isRelationalOperator(char *p);


int main(int argc, char ** argv)
{
	// Argument Handling
	char * filename;
	int print = 0;

	if (argc > 1)
	{
		if (strcmp(argv[1], "-a") == 0)
		{
			print = 1;
		}
	}

	token * head = getTokenList("tmp/lex.output", print);
	parser(head);
   return 0;
}


void parser(token * tok)
{
	printf("*** PARSER ***\n");

	program(tok);

   return;
}

token * program(token * tok)
{
	printf("\t***\tPROGRAM\t***\n");

	// get the first token after head
	tok = fetch(tok);

	// check for null token just incase empty list
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
	printf("\t***\tBLOCK\t***\n");

	if (tok->type == constsym)
	{
		// typically I'm adverse to do while loops, but in this case it is necessary
		do
		{
			tok = fetch(tok);

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

		} while(tok->type == commasym);

		if (tok->type != semicolonsym)
		{
			printf("semicolosym error in block/constsym\n");
			return NULL;
		}
	}

	// TODO: intsym is not defined or specified
	// int intsym = -69;

  // - Danish. Prof uses intsym and varsym interchangably in the sample symbol outputs.
  // Also in the sample pseudo code errors.
  // Therefore if we check for varsym rather than intsym we'll be good.

	if (tok->type == varsym)
	{
		do
		{
			tok = fetch(tok);
			if (tok->type != identsym)
			{
				printf("identsym error in block/insym\n");
				return NULL;
			}

			tok = fetch(tok);

		} while(tok->type == commasym);

		if (tok->type != semicolonsym)
		{
			printf("semicolosym error in block/insym\n");
			return NULL;
		}

		tok = fetch(tok);
	}

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

	if (tok->type == identsym)
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
		}
		tok = fetch(tok);
	}
	else if (tok->type == beginsym)
	{
		tok = fetch(tok);
		tok = statement(tok);

		while(tok->type == semicolonsym)
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
    // - Done.

    // Expression is supposed to check if the tok->value isn't relational operator.
    // Therefore isRelationalOperator returns 1 if it is, 0 if it isn't.
    // Using !, it reverses resulting in the appropriate check.
		if (!isRelationalOperator(tok->value))
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

  // !! WARNING TEMPORARY RETURN !! RETURN SOMETHING PROPER!.
  return NULL;
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
int isRelationalOperator(char *p)
{
  // Check if this is a relational operator.
  // If so return 1. If not return 0.

  if(strcmp(p,"<>") == 0)
    return 1;

  if(strcmp(p,":=") == 0 || strcmp(p,"=") == 0)
    return 1;

  if(strcmp(p,"<") == 0)
    return 1;

  if(strcmp(p,"<=") == 0)
    return 1;

  if(strcmp(p,">") == 0)
    return 1;

  if(strcmpy(p,">=") == 0)
    return 1;

  return 0;

}
