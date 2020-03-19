#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct
{
    int kind;       // const = 1, var = 2, proc = 3
    char name[12];  // name (up to 11 chars)
    int val;        // initial value
    int level;      // Lexicographical level
    int addr;       // Mem address
} symbol;

typedef struct token
{
    int type;
    char * name;
    struct token * next;
} token;

typedef enum
{
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6, slashsym = 7, oddsym = 8,
    eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
    commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23,
    thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
    readsym = 32, elsesym = 33

} token_type;

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

void parser(char * nameFile, char * typeFile);
void insertTail(token * head, char * name, int type);
void printList(token * head);
int getFileLen(char * filename);
char * fileToArr(char * filename);
void parser(char * nameFile, char * typeFile);
token * getTokenList(char * nameFile, char * typeFile);

int main(int argc, char ** argv)
{

    int MAX_SYMBOL_TABLE_SIZE = 256; // Need to check this
    symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

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
    int tokenLen = 256;
    // get length

	 token * list = getTokenList(nameFile, typeFile);
	 //printList(list);
    // Lex file IO




   // psuedocode implementation


    return;
}

token * getTokenList(char * nameFile, char * typeFile)
{


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

			//printf("%s\t%d\n", tmpStr, tmpType);

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

		token * tmp = head;

		// print list
		while(tmp != NULL)
		{
			printf("%s\t%d\n", tmp->name, tmp->type);
			tmp = tmp->next;
		}


	return head;
}


void insertTail(token * head, char * name, int type)
{
	token * tmp = head;

	// traverse to the end of the linked list
   while(tmp->next != NULL)
   {
       tmp = tmp->next;
   }

    // add a token at the end of the linked list
   tmp->next = malloc(sizeof(token));

	strcpy(tmp->next->name, name);
	//strcpy(tmp->next->type, type);
	//tmp->next->type = malloc(sizeof(int));
	tmp->next->type = type;
	tmp->next->next = NULL;

//	printf("%-12s\t\%s\n", tmp->name, tmp->type);

	 return;
}

void printList(token * head)
{
    token * tmp = head->next;

    while(tmp != NULL)
    {
        printf("%-12s\t\%s\n", tmp->name, tmp->type);
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
