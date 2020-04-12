// Written By:
// Stephen Speer
// Danish Siddiqui

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
token * lastToken = NULL;

int masterLevels = 0;
int stackidx = 0;
// parser functions
void parser(token * tok, symbol * head, instruction * code, int * cx);
token * block(int l, token * tok, symbol * head, instruction * code, int * cx);
token * program(token * tok, symbol * head, instruction * code, int * cx);
token * statement(int l, token * tok, symbol * head, instruction * code, int * cx, int reg);
token * expression(int l, token * tok, symbol * head, instruction * code, int * cx, int reg);
token * condition(int l, token * tok, symbol * head, instruction * code, int * cx, int reg);
token * factor(int l, token * tok, symbol * head, instruction * code, int * cx, int reg);
token * term(int l, token * tok,symbol * head, instruction * code, int * cx, int reg);

// symbol table
int isRelationalOperator(char *p);
void throwError(int err);
symbol * insertSym(symbol * sym, int kind, char * name, double val, int level, int addr);
symbol * createNewSymbolTable();
int setNewLevel(int level,symbolTableLevels * container);
int getNewAddress(int level,symbolTableLevels * container);
void printSymbolTable(symbol * head);
symbol * findVar(symbol * head, char * name);
int tableSize(symbol * head);
symbol * findByIndex(symbol * head, int index);

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
	printSymbolTable(symbolTableHead);
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
	printf("New Token: %s\n",tok->value);

	tok = block(0, tok, head, code, cx);

	if (tok->type != periodsym)
	{
		throwError(9);
		return NULL;
	}

	return tok;
}

token * block(int l, token * tok, symbol * head, instruction * code, int * cx)
{
	symbol * symbolTable;

	// allocate space in the stack
	stackidx = 0;

	int masterDoCount = 0;

	int tx0, cx0;
	tx0 = tableSize(head);

	if(tx0 < 1)
	{
		symbol * new = malloc(sizeof(symbol));
		new->kind = -1;
		strcpy(new->name,"new");
		new->val = -1;
		new->level = -1;
		new->addr = *cx;
		new->next = NULL;
		head->next = new;

	}
	else
	{
		symbolTable = findByIndex(head, tx0);
		symbolTable->addr = *cx;
	}

	do
	{
		symbolTableLevels * container = malloc(sizeof(symbolTableLevels));

		container->addressCount = 3;
		container->next = NULL;

		masterDoCount++;

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
				insertSym(head, 1, name, value, l, *cx);

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
				setNewLevel(l, container);
				insertSym(head, 2, name, value, l, getNewAddress(l,container));

				// allocate stack memory
				tok = fetch(tok);

				emit(code, cx, 6, 0, 0, 1);


			} while(tok->type == commasym);

			if (tok->type != semicolonsym)
			{
				throwError(10);
				return NULL;
			}

			tok = fetch(tok);
		}

		// !!!
		while(tok->type == procsym)
		{
			char * name = malloc(sizeof(char)*15);
			double value = 0;

			int cxtmp = *cx;
			emit(code, cx, 7, 0, 0, 0);

			tok = fetch(tok);

			if (tok->type != identsym)
			{
				throwError(4);
				return NULL;
			}

			strcpy(name,tok->value);

			// insert into symbol table.
			insertSym(head, 3, name, value, setNewLevel(l,container), *cx);

			tok = fetch(tok);

			if (tok->type != semicolonsym)
			{
				throwError(5);
				return NULL;
			}
			else
			{
				tok = fetch(tok);
			}

			// make a recursive call to this function. Increase lev.

			masterLevels++;
			tok = block(l+1, tok, head, code, cx);

			// return back to the line that's calling the procedure

			if (tok->type != semicolonsym)
			{
				throwError(5);
			}
			else
			{
				tok = fetch(tok);
			}

			emit(code, cx, 2, 0, 0, 0);

			// jump to end of procedure
			code[cxtmp].M = *cx;

		}

	}while((tok->type == constsym) || (tok->type == varsym) || (tok->type == procsym));

	int reg = 0;
	// code[symbolTable->addr].M = *cx;

	// allocate stack memory
	tok = statement(l, tok, head, code, cx, reg);

	// Return statement fucked up
	//emit(code, cx, 2, 0, 0, 0);
	return tok;
}

token * statement(int l, token * tok, symbol * head, instruction * code, int * cx, int reg)
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
			throwError(11);
			return NULL;
		}

		tok = fetch(tok);
		tok = expression(l, tok, head, code, cx, reg);

		// store the register into the stack
		emit(code, cx, 4, reg, var->level, var->addr);
	}
	else if (tok->type == callsym)
	{
		// check if this variable exists in symbol table.
		tok = fetch(tok);
		var = findVar(head,tok->value);

		if (tok->type != identsym)
		{
      	throwError(14); //call must be followed by an identifier
			return NULL;
    	}
		else
		{
			// check for valid addres
			if(var == NULL)
			{
				throwError(11);
				return NULL;
			}
			else if(var->kind == 3)
			{
				// This means it is a procsym.
				// put the emit statement here.
				emit(code, cx, 5, reg, l-(var->level), var->addr);
			}
			else
			{
				throwError(15);
				return NULL;
			}

			tok = fetch(tok);
		}
	}
	else if (tok->type == beginsym)
	{
		tok = fetch(tok);
		tok = statement(l,tok, head, code, cx, reg);

		while(tok->type == semicolonsym)
		{
			tok = fetch(tok);
			tok = statement(l,tok, head, code, cx, reg);
		}

		if (tok->type != endsym)
		{
			throwError(19);
			return NULL;
		}

		tok = fetch(tok);
	}
	else if (tok->type == ifsym)
	{
		tok = fetch(tok);
		tok = condition(l, tok, head, code, cx, reg);

		if (tok->type != thensym)
		{
			// printf("thensym error in statement/ifsym\n");
			throwError(16);
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
		tok = statement(l,tok, head, code, cx, reg);

		if(tok->type == elsesym)
		{
			tok = fetch(tok);

			code[cxtmp].M = *cx + 1;
			cxtmp = *cx ;

			emit(code, cx, 7, reg, 0, 0);
			tok = statement(l,tok, head, code, cx, reg);

		}

		// change the jump to the line fo code for the statemnet
		code[cxtmp].M = *cx;
	}
	else if (tok->type == whilesym)
	{
		// beggining of loop
		int cx1 = *cx;

		tok = fetch(tok);
		tok = condition(l, tok, head, code, cx, reg);

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

		tok = statement(l,tok, head, code, cx, reg);

		// jump unconditionally to beggining of loop
		emit(code, cx, 7, 0, 0, cx1);

		// change cx2 so we can break loop
		code[cx2].M = *cx;
	}
	else if (tok->type == readsym)
	{
		// get the variable name
		tok = fetch(tok);

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

		emit(code, cx, 4, reg, tmp->level, tmp->addr);

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

token * condition(int l, token * tok, symbol * head, instruction * code, int * cx, int reg)
{

	if (tok->type == oddsym)
	{
		tok = fetch(tok);
		tok = expression(l,tok, head, code, cx, reg);
	}
	else
	{
		tok = expression(l,tok, head, code, cx, reg);

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
			tok = expression(l,tok, head, code, cx, reg+1);
			emit(code, cx, rel, reg, reg, reg+1);
		}
	}
	return tok;
}

token * expression(int l, token * tok, symbol * head, instruction * code, int * cx, int reg)
{
	token * addop;

	if (tok->type == plussym || tok->type == minussym)
	{
		addop = tok;
		tok = fetch(tok);
		tok = term(l, tok, head, code, cx, reg);

		// negation
		if (addop->type == minussym)
		{
			// OP OPR_NEG = 12
			emit(code, cx, 12, reg, 0, reg);
		}
	}
	else
	{
		tok = term(l, tok, head, code, cx, reg);
	}

	while(tok->type == plussym || tok->type == minussym)
	{
		addop = tok;

		tok = fetch(tok);
		// load the next value in at the second index
		tok = term(l,tok, head, code, cx, reg+1);

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

token * term(int l,token * tok,symbol * head, instruction * code, int * cx, int reg)
{
	token * mulop;

	tok = factor(l,tok, head, code, cx, reg);

	while(tok->type == multsym || tok->type == slashsym)
	{
		mulop = tok;

		tok = fetch(tok);
		tok = factor(l,tok, head, code, cx, reg+1);

		// multiplication
		if (mulop->type == multsym)
		{
			// OP Multiply = 15
			emit(code, cx, 15, reg, 0, reg+1);
		}
		// division
		else
		{
			// OP Division = 16
			emit(code, cx, 16, reg, 0, reg+1);
		}
	}

  return tok;
}

token * factor(int l, token * tok, symbol * head, instruction * code, int * cx, int reg)
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
			emit(code, cx, 3, reg, tmp->level, tmp->addr);
		}

		tok = fetch(tok);
	}
	else if (tok->type == numbersym)
	{
		// put numbers into register using lit
		emit(code, cx, 1, reg, 0, atoi(tok->value));
		tok = fetch(tok);
	}
	else if (tok->type == lparentsym)
	{
		tok = fetch(tok);
		tok = expression(l, tok, head, code, cx, reg);

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
		return NULL;
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


		if(kind == 2)
		{
			tmp->addr = stackidx;
			stackidx++;
		}
		else if(kind == 3)
		{
			tmp->addr = addr;
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

int tableSize(symbol * head)
{
	symbol * temp = head->next;
	int count = 0;

	while(temp!=NULL)
	{
		temp = temp->next;
		count++;
	}

	return count;
}

symbol * findByIndex(symbol * head, int index)
{
	symbol * temp = head;

	while(index>0)
	{
		temp = temp->next;
		index--;
	}

	return temp;
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
	  return NULL;
	}

	printf("VARIABLE: %s %d\n", temp->name, temp->addr);
	// return pointer to symbol
	return temp;
}

int setNewLevel(int level, symbolTableLevels * container)
{
  // iterate to last level.
  symbolTableLevels * t = container;
  int i;

  for(i = 0; i < level; i++)
  {
	 if(t->next == NULL)
	 {
		// if no further container exits, create a level and add it.
		symbolTableLevels * new = malloc(sizeof(symbolTableLevels));

		new->addressCount = 3;
		new->next = NULL;
		t->next = new;
	 }

	 t = t->next;
  }
	printf("New Level Set. ");
  return level;
}

// problematic. Setting wrong address to variables
int getNewAddress(int level, symbolTableLevels * container)
{
  // iterate to last level.
  symbolTableLevels * t = container;
  int i;

  for(i = 0; i < level; i++)
	 t = t->next;

	// this is probably wrong
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
