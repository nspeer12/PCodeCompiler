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
	 double val;     // initial value
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

#define MAX_CODE_SIZE 99999

int levelCount = 0;

// parser functions
void parser(token * tok, symbol * head, instruction * code, int * cx);
token * block(token * tok, symbol * head, instruction * code, int * cx);
token * program(token * tok, symbol * head, instruction * code, int * cx);
token * statement(token * tok, symbol * head, instruction * code, int * cx, int reg);
token * expression(token * tok, symbol * head, instruction * code, int * cx, int reg);
token * condition(token * tok, symbol * head, instruction * code, int * cx, int reg);
token * factor(token * tok, symbol * head, instruction * code, int * cx, int reg);
token * term(token * tok,symbol * head, instruction * code, int * cx, int reg);

// symbol table
int isRelationalOperator(char *p);
void throwError(int err);
symbol * insertSym(symbol * sym, int kind, char * name, double val, int level, int addr);
symbol * createNewSymbolTable();
int setNewLevel(int level,symbolTableLevels * levels);
int getNewAddress(int level,symbolTableLevels * levels);
void printSymbolTable(symbol * head);
symbol * findVar(symbol * head, char * name);

// code generation
void printCode(instruction * code, int * cx, int print);
void emit(instruction * code, int * cx, int op, int R, int L, int M);

int main(int argc, char ** argv)
{
	// takes ouput from lex as input
	char * filename = "tmp/lex.output";

	// print argument
	int print = (argc > 1 && (strcmp(argv[1], "-a") == 0)) ? 1 : 0;
	//int print = 0;
	// store code in array
	instruction code [MAX_CODE_SIZE];
	// code index counter
	int * cx = malloc(sizeof(int));
	*cx = 0;

	symbol * symbolTableHead = createNewSymbolTable();
	token * head = getTokenList(filename, print);

	// begin recursive descent parsing
	parser(head, symbolTableHead, code, cx);

	if (print)
		printf("No errors, program is syntactically correct\n");

	printCode(code, cx, print);

	return 0;
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

	tok = block(tok, head, code, cx);

	if (tok->type != periodsym)
	{
		throwError(9);
		return NULL;
	}

	return tok;
}

token * block(token * tok, symbol * head, instruction * code, int * cx)
{
  symbolTableLevels * levels = malloc(sizeof(symbolTableLevels));

  levels->addressCount = 3;
  levels->next = NULL;

	// allocate space in the stack
	int stackidx = 4;

	if (tok->type == constsym)
	{
		char * name = malloc(sizeof(char)*15);
		double value = 0;

		do
		{
			tok = fetch(tok);

			if (tok->type != identsym)
			{
				throwError(4);
				return NULL;
			}
			else
			{
				// set the name variable.
				strcpy(name,tok->value);
				tok = fetch(tok);
			}

			if (tok->type != eqlsym)
			{
				throwError(3);
				return NULL;
			}
			else
			{
				tok = fetch(tok);
			}

			if (tok->type != numbersym)
			{
				throwError(2);
				return NULL;
			}
			else
			{
				value = atof(tok->value);
				tok = fetch(tok);
			}

			// add constant to table
			// -1 for null addresses.
			insertSym(head, 1, name, value, -1, -1);

		} while(tok->type == commasym);

		if (tok->type != semicolonsym)
		{
			throwError(5);
			return NULL;
		}

		tok = fetch(tok);
	}

	// Prof uses intsym and varsym interchangably in the sample symbol outputs.
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

			// insert variable into symbol table.
			insertSym(head, 2, name, value, setNewLevel(levelCount,levels), getNewAddress(levelCount,levels));
			tok = fetch(tok);

			// allocate stack memory
			emit(code, cx, 6, 0, 0, 1);

		} while(tok->type == commasym);

		if (tok->type != semicolonsym)
		{
			throwError(10);
			return NULL;
		}

		tok = fetch(tok);
	}

	int reg = 0;
	tok = statement(tok, head, code, cx, reg);

	return tok;
}

token * statement(token * tok, symbol * head, instruction * code, int * cx, int reg)
{
	symbol * var;

	if (tok->type == identsym)
	{
		 // check if this variable exists in symbol table.
		 var = findVar(head,tok->value);

		 // check for valid addres
		 if(var == NULL)
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
		tok = expression(tok, head, code, cx, reg);

		// store the register into the stack
		emit(code, cx, 4, reg, 0, var->addr);
	}
	else if (tok->type == beginsym)
	{
		tok = fetch(tok);
		tok = statement(tok, head, code, cx, reg);

		while(tok->type == semicolonsym)
		{
			tok = fetch(tok);
			tok = statement(tok, head, code, cx, reg);
		}

		if (tok->type != endsym)
		{
			throwError(19);
			return NULL;
		}

		// decrement the level integer
		if(levelCount > 0)
		{
			levelCount--;
		}

		tok = fetch(tok);
	}
	else if (tok->type == ifsym)
	{
		tok = fetch(tok);
		tok = condition(tok, head, code, cx, reg);

		if (tok->type != thensym)
		{
			// printf("thensym error in statament/ifsym\n");
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}

		// jump for statement
		int cxtmp = *cx;

		// OP JPC == 8
		emit(code, cx, 8, reg, 0, 0);
		tok = statement(tok, head, code, cx, reg);

		// change the jump to the line fo code for the statemnet
		code[cxtmp].M = *cx;
	}
	else if (tok->type == whilesym)
	{
		// beggining of loop
		int cx1 = *cx;

		tok = fetch(tok);
		tok = condition(tok, head, code, cx, reg);

		// to control jump
		int cx2 = *cx;
		// jpc to break loop
		emit(code, cx, 8, reg, 0, 0);

		if (tok->type != dosym)
		{
			throwError(18);
			return NULL;
		}
		else
		{
			tok = fetch(tok);
		}


		tok = statement(tok, head, code, cx, reg);

		// jump unconditionally to beggining of loop
		emit(code, cx, 7, 0, 0, cx1);

		// change cx2 so we can break loop
		code[cx2].M = *cx;
	}
	else if (tok->type == readsym)
	{
		// TODO: All of this

		// read

		// get the variable name
		tok = fetch(tok);

		printf("TOK TYPE %d", tok->type);

		// OP in to register is 10
		emit(code, cx, 10, reg, 0, reg);

		// store in variable

		/* find variable address and store it in */

		symbol * tmp = findVar(head, tok->value);
		if (tmp == NULL)
		{
			// variable does not exist
			throwError(11);
		}

		emit(code, cx,4, reg, 0, tmp->addr);

		tok = fetch(tok);
		if (tok->type != semicolonsym)
		{
			throwError(5);
			return NULL;
		}

	}
	else if (tok->type == writesym)
	{
		// write
		tok = fetch(tok);
		symbol * tmp = findVar(head, tok->value);
		emit(code, cx, 3, reg, 0, tmp->addr);
		emit(code, cx, 9, reg, 0, reg);

		tok = fetch(tok);
		if (tok->type != semicolonsym)
		{
			throwError(5);
			return NULL;
		}
	}

	return tok;
}

token * condition(token * tok, symbol * head, instruction * code, int * cx, int reg)
{

	if (tok->type == oddsym)
	{
		tok = fetch(tok);
		tok = expression(tok, head, code, cx, reg);
	}
	else
	{
		tok = expression(tok, head, code, cx, reg);

		// store relation operator
		int rel = isRelationalOperator(tok->value);
		if (rel == 0)
		{
			throwError(20);
			return NULL;
		}
		else
		{
			tok = fetch(tok);
			tok = expression(tok, head, code, cx, reg+1);
			emit(code, cx, rel, reg, reg, reg+1);
		}
	}

	return tok;
}

token * expression(token * tok, symbol * head, instruction * code, int * cx, int reg)
{
	token * addop;

	if (tok->type == plussym || tok->type == minussym)
	{
		addop = tok;
		tok = fetch(tok);
		tok = term(tok, head, code, cx, reg);

		// negation
		if (addop->type == minussym)
		{
			// emit (OP, 0, OPR_NEG)
			// OP OPR_NEG = 12
			emit(code, cx, 12, reg, 0, reg);
		}
	}
	else
	{
		tok = term(tok, head, code, cx, reg);
	}

	while(tok->type == plussym || tok->type == minussym)
	{
		addop = tok;

		tok = fetch(tok);
		// load the next value in at the second index
		tok = term(tok, head, code, cx, reg+1);

		if (addop->type == plussym)
		{
			// OP Add = 13
			// add two numbers
			emit(code, cx, 13, reg, reg, reg+1);
		}
		else if (addop->type == minussym)
		{
			// OP 14 is subtraction
			emit(code, cx, 14, reg, reg, reg+1);
		}
	}

	return tok;
}

token * term(token * tok,symbol * head, instruction * code, int * cx, int reg)
{
	token * mulop;

	tok = factor(tok, head, code, cx, reg);

	while(tok->type == multsym || tok->type == slashsym)
	{
		mulop = tok;

		tok = fetch(tok);
		tok = factor(tok, head, code, cx, reg+1);

		// multiplication
		if (mulop->type == multsym)
		{
			// OP Multiply = 15
			emit(code, cx, 15, reg, reg, reg+1);
		}
		// division
		else
		{
			// OP Division = 16
			emit(code, cx, 16, reg, reg, reg+1);
		}
	}

  return tok;
}

token * factor(token * tok, symbol * head, instruction * code, int * cx, int reg)
{

	if (tok->type == identsym)
	{
		// search for constant
		symbol * tmp = findVar(head, tok->value);

		// check if the value was a constant in the symbol table
		if (tmp == NULL)
		{
			// variable not found
			throwError(11);
		}
		else if (tmp->kind == 1) // constant
		{
			emit(code, cx, 1, reg, 0, tmp->val);
		}
		else if (tmp->kind == 2) // variable
		{
			// LOD from stack into register
			emit(code, cx, 3, reg, 0, tmp->addr);
		}

		tok = fetch(tok);
	}
	else if (tok->type == numbersym)
	{
		// PUT NUMBER INTO REGISTER USING LIT
		emit(code, cx, 1, reg, 0, atoi(tok->value));
		tok = fetch(tok);
	}
	else if (tok->type == lparentsym)
	{
		tok = fetch(tok);
		tok = expression(tok, head, code, cx, reg);
		if (tok->type != rparentsym)
		{
			throwError(22);
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

	 // need to account for addresses
	 // NOTE: start at address -1, since
	 int i = 0;
	 while(iterator->next != NULL)
	 {
		iterator = iterator->next;

		// only account for varaibles
		if (iterator->kind == 2)
			i++;
	 }

		symbol * tmp = malloc(sizeof(symbol));
		tmp->kind = kind;
		strcpy(tmp->name, name);
		tmp->val = val;
		tmp->level = level;


		// allow for custom insertions into symbol table
		if (addr < 0)
		{
			// use counter variable
			tmp->addr = addr;
		}
		else
		{
			tmp->addr = i;
		}

		tmp->next = NULL;
		iterator->next = tmp;

		return sym;

}

int isRelationalOperator(char * p)
{
	// Check if this is a relational operator.
	// If so return 1. If not return 0.
	// returns the operation code

	// i have no clue what <> does, assuming for odd operation
	if(strcmp(p,"odd") == 0)
	 	return 17;

	// not equal to
	if(strcmp(p,"<>") == 0)
		return 20;

  	if(strcmp(p,"=") == 0)
	 	return 19;

	if(strcmp(p,":=") == 0)
		throwError(1);

	if(strcmp(p,"<") == 0)
		return 21;

	if(strcmp(p,"<=") == 0)
		return 22;

 	if(strcmp(p,">") == 0)
		return 23;

 	if(strcmp(p,">=") == 0)
		return 24;

  return 0;
}

void throwError(int err)
{
	switch(err)
	{
		case 1:
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
			perror("Error: Statement expected\n");
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
			perror("Error: Call of constant or variable is meaningless\n");
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

	exit(1);
}

symbol * findVar(symbol * head, char * name)
{
	// start after the head
	symbol * temp = head->next;

	while(temp != NULL && (strcmp(temp->name,name) != 0))
	{
	  temp = temp->next;
	}

	if(temp == NULL)
	{
	  // error the variable we are referring to in the arithmetic doesn't even exist.
	  printf("variable was not found");
	  return NULL;
	}

	// return pointer to symbol
	return temp;
}

int setNewLevel(int level, symbolTableLevels * levels)
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

int getTableIndex(symbol * head)
{
	int i = 0;
	symbol * tmp = head;

	while(tmp != NULL)
	{
		tmp = tmp->next;
		i++;
	}

	return i;
}

void printSymbolTable(symbol * head)
{
	printf("\nSymbol Table\n");

	symbol * tmp = head->next;

	while(tmp != NULL)
	{
		printf("%d\t%s\t%.2f\t%d\t%d\n", tmp->kind, tmp->name, tmp->val, tmp->level, tmp->addr);
		tmp = tmp->next;
	}
}

void printCode(instruction * code, int * cx, int print)
{
	FILE * fp = fopen("tmp/instructions.a", "w");

	if (print == 1)
	{
		printf("\nCODE\n");
		printf("OP\tR\tL\tM\n");
	}

	for (int i=0; i < *cx; i++)
	{
		if (print == 1)
			printf("%d\t%d\t%d\t%d\n", code[i].op, code[i].R, code[i].L, code[i].M);

		fprintf(fp, "%d %d %d %d\n", code[i].op, code[i].R, code[i].L, code[i].M);
	}
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
