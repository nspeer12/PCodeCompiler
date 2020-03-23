#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
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
void throwError(int err);


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
	program(tok);
   return;
}

token * program(token * tok)
{
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
		{
			// missing period
			throwError(9);
			return NULL;
		}
	}

  	return tok;
}

token * block(token * tok)
{
	if (tok->type == constsym)
	{
		do
		{
			tok = fetch(tok);

			if (tok->type != identsym)
			{

				// printf("identsym error in block/constsym");
				return NULL;
			}

			tok = fetch(tok);

			if (tok->type != eqlsym)
			{
				//printf("eqsym error in block/constsym\n");
				return NULL;
			}

			tok = fetch(tok);

			if (tok->type != numbersym)
			{
				// insert into symbol table??
				//printf("numbersym error in block/constsym\n");
				return NULL;
			}

			tok = fetch(tok);

		} while(tok->type == commasym);

		if (tok->type != semicolonsym)
		{
			//printf("semicolosym error in block/constsym\n");
			return NULL;
		}
	}
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
				//printf("identsym error in block/insym\n");
				return NULL;
			}

			tok = fetch(tok);

		} while(tok->type == commasym);

		if (tok->type != semicolonsym)
		{
			//printf("semicolosym error in block/insym\n");
			return NULL;
		}

		tok = fetch(tok);
	}

	while(tok->type == procsym)
	{
		tok = fetch(tok);
		if (tok->type != identsym)
		{
			//printf("identsym error in block/procsym\n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}

		if (tok->type != semicolonsym)
		{
			//printf("semicolonsym error in block/procsym\n");
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

	//	printf("\t***\tSTATEMENT\t***\n");

	if (tok->type == identsym)
	{
		tok = fetch(tok);

		if (tok->type != becomessym)
		{
			// printf("becomessym error in statement/indentsym: %s\n", tok->name);
			return NULL;
		}

		tok = fetch(tok);
		tok = expression(tok);
		// ** ERROR LOCATION **
		// added in this line, partially working for now
		// basically, there is a fetch missing that is causing errors
		// tok = fetch(tok);

	}
	else if (tok->type == callsym)
	{
		tok = fetch(tok);

		if (tok->type != identsym)
		{
			// printf("identsym err in statement/callsym\n");
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
			// printf("endsym err in statement/beginsym: %s\n", tok->name);
			return NULL;
		}

		tok = fetch(tok);
	}
	else if (tok->type == ifsym)
	{
		tok = fetch(tok);
		tok = condition(tok);

		if (tok->type != thensym)
		{
			// printf("thensym error in statament/ifsym\n");
			return NULL;
		}

		tok = fetch(tok);
		tok = statement(tok);
	}
	else if (tok->type == whilesym)
	{
		tok = fetch(tok);
		tok = condition(tok);

		if (tok->type != dosym)
		{
			// printf("dosym error in statement/whilesym\n");
			return NULL;
		}

		tok = fetch(tok);
		tok = statement(tok);
	}

	return tok;
}

token * condition(token * tok)
{

//	printf("\t***\tCONDITION\t***\n");

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
			// printf("relation error in condition/else \n");
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

	// printf("\t***\tEXPRESSION\t***\n");

	if (tok->type == plussym || tok->type == minussym)
	{
		tok = fetch(tok);
	}

	tok = term(tok);

	while(tok->type == plussym || tok->type == minussym)
	{
		tok = fetch(tok);
		tok = term(tok);
	}

	return tok;
}

token * term(token * tok)
{
	// printf("\t***\tTERM\t***\n");

	tok = factor(tok);

	while(tok->type == multsym || tok->type == slashsym)
	{
		tok = fetch(tok);
		tok = factor(tok);
	}

  return tok;
}

token * factor(token * tok)
{

	// printf("\t***\tFACTOR\t***\n");

	if (tok->type == identsym)
	{
		tok = fetch(tok);
	}
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
			// printf("missing ) in factor/(\n");
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

  if(strcmp(p,">=") == 0)
    return 1;

  return 0;

}

void throwError(int err)
{
	switch(err)
	{
		case 1:
			// wtf ...
			perror("Error: Use = instead of :=\n");
			break;
		case 2:
			perror("Error:  = must be followed by a number\n");
			break;
		case 3:
			perror("Error: Identifier must be followed by a =\n");
			break;
		case 4:
			perror("Error: const, var, procedure must be followed by an identifier\n");
			break;
		case 5:
			perror("Error: Semicolon or comma missing\n");
			break;
		case 6:
			perror("Error: Incorrect symbol after procedure delaration.\n");
			break;
		case 7:
			perror("Error: Statemetn expected\n");
			break;
		case 8:
			perror("Error: Incorrect symbol after statement part in block\n");
			break;
		case 9:
			perror("Error: Period expected\n");
			break;
		case 10:
			perror("Error: Semicolon between statements missing\n");
			break;
		case 11:
			perror("Error: Undeclared identifier\n");
			break;
		case 12:
			perror("Error: Assignment to constant or procedure is not allowed\n");
			break;
		case 13:
			perror("Error: Assignment operator expected\n");
			break;
		case 14:
			perror("Error: Call must be folloed by an identifier\n");
			break;
		case 15:
			perror("Error: Call of constant or variabl is meaningless\n");
			break;
		case 16:
			perror("Error: then expected\n");
			break;
		case 17:
			perror("Error: Semicolon or } expected\n");
			break;
		case 18:
			perror("Error: do expected\n");
			break;
		case 19:
			perror("Error: Incorrect symbol following statement\n");
			break;
		case 20:
			perror("Error: Relational operator expected");
			break;
		case 21:
			perror("Error: Expression must not contain a procedure identifier\n");
			break;
		case 22:
			perror("Error: Right parenthesis missing\n");
			break;
		case 23:
			perror("Error: The preceding factor cannot begin with this symbol\n");
			break;
		case 24:
			perror("Error: An expression cannot begin with a symbol\n");
			break;
		case 25:
			perror("Error: The number is too large\n");
			break;
		default:
			perror("Error\n");
			break;
	}

	return;
}
