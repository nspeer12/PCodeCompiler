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
    double val;        // initial value
    int level;      // Lexicographical level
    int addr;       // Mem address
	 struct symbol * next; // for linked list
} symbol;

typedef struct symbolTableLevels
{
  int addressCount;
  struct symbolTableLevels * next;

} symbolTableLevels;

typedef struct instruction
{
	int op;	// Operation
	int R;	// Register
	int L;	// Lexicographical level
	int M;	// Modifier
} instruction;

#define MAX_CODE_SIZE 6969

int levelCount = 0;
token * fetch(token * tok);

// parser functions
void parser(token * tok, symbol * head, instruction * code, int * cx);
token * block(token * tok, symbol * head, instruction * code, int * cx);
token * program(token * tok, symbol * head, instruction * code, int * cx);
token * statement(token * tok, symbol * head, instruction * code, int * cx);
token * expression(token * tok, symbol * head, instruction * code, int * cx);
token * condition(token * tok, symbol * head, instruction * code, int * cx);
token * factor(token * tok, symbol * head, instruction * code, int * cx);
token * term(token * tok,symbol * head, instruction * code, int * cx);
int isRelationalOperator(char *p);
void throwError(int err);
int findVar(symbol *head, char *name);
symbol * insertSym(symbol * sym, int kind, char * name, double val, int level, int addr);
symbol * createNewSymbolTable();
int setNewLevel(int level,symbolTableLevels * levels);
int getNewAddress(int level,symbolTableLevels * levels);

// code generation
void printCode(instruction * code, int * cx);
void emit(instruction * code, int * cx, int op, int R, int L, int M);

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

	// store code in array
	instruction * code = malloc(sizeof(instruction) * MAX_CODE_SIZE);
	// keep track of current instruction
	int * cx = malloc(sizeof(int));
	*cx = 0;

	symbol * symbolTableHead = createNewSymbolTable();
  	token * head = getTokenList("tmp/lex.output", print);

	parser(head, symbolTableHead, code, cx);
	printCode(code, cx);

	return 0;
}

// adds a line of code to our array
void emit(instruction * code, int * cx, int op, int R, int L, int M)
{
	if (*cx > MAX_CODE_SIZE)
	{

		throwError(25);
	}
	else
	{
		code[*cx].op = op;
		code[*cx].R = R;
		code[*cx].L = L;
		code[*cx].M = M;
		*cx = *cx + 1;
	}
}

void printCode(instruction * code, int * cx)
{
	printf("\nCODE\n");
	printf("OP\tR\tL\tM\n");

	for (int i=0; i < *cx; i++)
	{
		printf("%d\t%d\t%d\t%d\n", code[i].op, code[i].R, code[i].L, code[i].M);
	}
}

void parser(token * tok, symbol * head, instruction * code, int * cx)
{
	program(tok, head, code, cx);
   return;
}

token * program(token * tok, symbol * head, instruction * code, int * cx)
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
		tok = block(tok, head, code, cx);

		if (tok->type != periodsym)
		{
			// missing period
			throwError(9);
			return NULL;
		}
	}

  	return tok;
}

token * block(token * tok, symbol * head, instruction * code, int * cx)
{
  symbolTableLevels * levels = malloc(sizeof(symbolTableLevels));

  levels->addressCount = 3;
  levels->next = NULL;

	if (tok->type == constsym)
	{
    char * name = malloc(sizeof(char)*15);
    double value = 0;

		do
		{
			tok = fetch(tok);

			if (tok->type != identsym)
			{

				// printf("identsym error in block/constsym");
				return NULL;
			}
      else
      {
        // set the name variable.
        strcpy(name,tok->value);
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
      else
      {
        // set the value variable.
        value = atof(tok->value);
      }

			tok = fetch(tok);

      // everything is good to go add this constant into the table.
      // symbol * insertSym(symbol * sym, int kind, char * name, double val, int level, int addr);
      // -1 for null addresses.

      insertSym(head, 1, name, value, -1, -1);

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
    char * name = malloc(sizeof(char)*15);
    double value = 0;

		do
		{
			tok = fetch(tok);
			if (tok->type != identsym)
			{
				//printf("identsym error in block/insym\n");
				return NULL;
			}
      strcpy(name,tok->value);

      // everything good to go.
      // variable has been declared.
      // insert into symbol table.
      // symbol * insertSym(symbol * sym, int kind, char * name, double val, int level, int addr);
      insertSym(head, 2, name, value, setNewLevel(levelCount,levels), getNewAddress(levelCount,levels));

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
    char * name = malloc(sizeof(char)*15);
    double value = 0;

		tok = fetch(tok);
		if (tok->type != identsym)
		{
			//printf("identsym error in block/procsym\n");
			return NULL;
		}

    strcpy(name,tok->value);

    // insert into symbol table.
    insertSym(head, 3, name, value, setNewLevel(levelCount,levels), -1);

    // increase level count after adding the procedure.
    levelCount++;

    tok = fetch(tok);

		if (tok->type != semicolonsym)
		{
			//printf("semicolonsym error in block/procsym\n");
			return NULL;
		}

    tok = fetch(tok);

	}

	tok = statement(tok, head, code, cx);

	return tok;
}

token * statement(token * tok, symbol * head, instruction * code, int * cx)
{

	//	printf("\t***\tSTATEMENT\t***\n");

	if (tok->type == identsym)
	{
    // check if this variable exists in symbol table.
    // if it doesn't throw error.

    if(!findVar(head,tok->value))
    {
      // printf("Variable identifier being used doesn't exist in the symbol table. ");
      return NULL;
    }

		tok = fetch(tok);

		if (tok->type != becomessym)
		{
			// printf("becomessym error in statement/indentsym: %s\n", tok->name);
			return NULL;
		}

		tok = fetch(tok);
		tok = expression(tok, head, code, cx);

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
		tok = statement(tok, head, code, cx);

		while(tok->type == semicolonsym)
		{
			tok = fetch(tok);
			tok = statement(tok, head, code, cx);
		}


		if (tok->type != endsym)
		{
			// printf("endsym err in statement/beginsym: %s\n", tok->name);
			return NULL;
		}

    // decrement the level integer value.
    if(levelCount > 0)
    {
      levelCount--;
    }

    tok = fetch(tok);
	}
	else if (tok->type == ifsym)
	{
		tok = fetch(tok);
		tok = condition(tok, head, code, cx);

		if (tok->type != thensym)
		{

			// printf("thensym error in statament/ifsym\n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}

		// jump for if condition
		int cxtmp = *cx;

		// OP JMP = 7
		emit(code, cx, 7, 0, 0, 0);
		tok = statement(tok, head, code, cx);
		code[cxtmp].M = *cx;
	}
	else if (tok->type == whilesym)
	{
		int cx1 = *cx;

		tok = fetch(tok);
		tok = condition(tok, head, code, cx);

		int cx2 = *cx;

		// OP JPC = 8
		emit(code, cx, 8, 0, 0, 0);

		if (tok->type != dosym)
		{
			// printf("dosym error in statement/whilesym\n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}
		tok = statement(tok, head, code, cx);

		// jump to beginning
		// OP JMP = 7
		emit(code, cx, 7, 0, 0, cx1);
		code[cx2].M = *cx;
	}

	return tok;
}

token * condition(token * tok, symbol * head, instruction * code, int * cx)
{

//	printf("\t***\tCONDITION\t***\n");

	// this may be wrong, per his psuedocode
	if (tok->type == oddsym)
	{
		tok = fetch(tok);
		tok = expression(tok, head, code, cx);
	}
	else
	{
		tok = expression(tok, head, code, cx);
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
			tok = expression(tok, head, code, cx);
		}
	}

	return tok;
}

token * expression(token * tok, symbol * head, instruction * code, int * cx)
{
	token * addop;
	// printf("\t***\tEXPRESSION\t***\n");

	if (tok->type == plussym || tok->type == minussym)
	{
		addop = tok;
		tok = fetch(tok);
		tok = term(tok, head, code, cx);

		// negation
		if (addop->type == minussym)
		{
			// emit (OP, 0, OPR_NEG)
			// OP OPR_NEG = 12
			emit(code, cx, 12, 0, 0, 0);
		}
	}
	else
	{
		tok = term(tok, head, code, cx);
	}

	while(tok->type == plussym || tok->type == minussym)
	{
		addop = tok;

		tok = fetch(tok);
		tok = term(tok, head, code, cx);

		if (addop->type == plussym)
		{
			// emit(OP, 0, OPR_ADD)
			// OP Add = 13
			emit(code, cx, 13, 0, 0, 0);
		}
		else if (addop->type == minussym)
		{
			// emit (OPR, 0, OPR_SUB)
			// OP 14 is subtraction
			emit(code, cx, 14, 0, 0, 0);
		}
	}

	return tok;
}


token * term(token * tok,symbol * head, instruction * code, int * cx)
{
	// printf("\t***\tTERM\t***\n");
	token * mulop;

	tok = factor(tok, head, code, cx);

	while(tok->type == multsym || tok->type == slashsym)
	{
		mulop = tok;

		tok = fetch(tok);
		tok = factor(tok, head, code, cx);

		// multiplication
		if (mulop->type == multsym)
		{
			// OP Multiply = 15
			emit(code, cx, 15, 0, 0, 0);
		}
		// division
		else
		{
			// OP Division = 16
			emit(code, cx, 16, 0, 0, 0);
		}
	}

  return tok;
}

token * factor(token * tok, symbol * head, instruction * code, int * cx)
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
		tok = expression(tok, head, code, cx);
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

symbol * createNewSymbolTable()
{
  /*
  typedef struct symbol
  {
      int kind;       // const = 1, var = 2, proc = 3
      char name[12];  // name (up to 11 chars)
      double val;        // initial value
      int level;      // Lexicographical level
      int addr;       // Mem address
  	 struct symbol * next; // for linked list
  } symbol;
  */

  symbol * head = malloc(sizeof(symbol));
  head->kind = -1;
  strcpy(head->name,"head");
  head->val = -1;
  head->level = -1;
  head->addr = -1;
  head->next = NULL;

  return head;

}

symbol * insertSym(symbol * sym, int kind, char * name, double val, int level, int addr)
{
    symbol * iterator = sym;

    while(iterator->next != NULL)
    {
      iterator = iterator->next;
    }

		symbol * tmp = malloc(sizeof(symbol));
		tmp->kind = kind;
		strcpy(tmp->name, name);
		tmp->val = val;
		tmp->level = level;
		tmp->addr = -69;
		tmp->next = NULL;
    iterator->next = tmp;

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

int findVar(symbol *head, char *name)
{
  symbol *temp = malloc(sizeof(symbol));
  temp = head;

  while((strcmp(temp->name,name) != 0) && temp != NULL)
  {
    temp = temp->next;
  }

  if(temp == NULL)
  {
    // error the variable we are referring to in the arithmetic doesn't even exist.
    return 0;
  }

  return 1;
}

int setNewLevel(int level,symbolTableLevels * levels)
{
  // iterate to last level.
  symbolTableLevels * t = levels;
  int i;

  for(i = 0; i < level; i++)
  {
    if(t->next == NULL)
    {
      // if no further levels exits, create a level and add it.
      symbolTableLevels * new = malloc(sizeof(symbolTableLevels));

      new->addressCount = 3;
      new->next = NULL;
      t->next = new;
    }

    t = t->next;
  }
  return level;
}

int getNewAddress(int level,symbolTableLevels * levels)
{
  // iterate to last level.
  symbolTableLevels * t = levels;
  int i;

  for(i = 0; i < level; i++)
    t = t->next;

  t->addressCount++;

  return t->addressCount;
}
