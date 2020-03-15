#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
    char type[12];
    char name[12];
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

void parser(char * filename);
void insertTail(token * head, char * name);
void printList(token * head);

int main(int argc, char ** argv)
{

    int MAX_SYMBOL_TABLE_SIZE = 256; // Need to check this
    symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];


    // needs to take in flags
        // -l: print the list of lexemes/tokens
        // -a: print the generated assembly code (parser/codegen output)
        // -v: print virtual machine execution trace
    bool lex, asmbl, vm = 0;


	 /*
	 *** COMMAND LINE ARG HANDLING
    // check for inputs and assign flags
    for (int i = 1; i < argc; i++)
    {
        if (strcmp("-l", argv[i]) == 0)
        {
            lex = true;
        }
        else if (strcmp("-a", argv[i]) == 0)
        {
            asmbl = true;
        }
        else if (strcmp("-v", argv[i]) == 0)
        {
            vm = true;
        }
        else
        {
            printf("Please enter valid flags:\n");
            printf("\t-l: print the list of lexemes/tokens\n");
            printf("\t-a: print the generated assembly code\n");
            printf("\t-v: print the virtual machine execution trace\n");
        }
    }
	 */

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


    parser("tmp/lex.output.txt");

    return 0;
}

void parser(char * filename)
{
    // dynamically allocated based on how many tokens there are
    int tokenLen = 256;
    // get length
    int fileLen = 0;

    FILE * fp = fopen(filename, "r");

    while(!feof(fp))
    {
        fgetc(fp);
        fileLen++;
    }
    fclose(fp);


    // Lex file IO
    char * lexInput = malloc(sizeof(char) * fileLen);
    fp = fopen(filename, "r");


	 int i = 0;
    char tmp;
    while(!feof(fp))
    {
        tmp = fgetc(fp);
        if (tmp == EOF)
            break;
        else
        {
            lexInput[i++] = tmp;
        }

        printf("%c", tmp);
    }

    // create a new linked list to store tokens
    token * head = malloc(sizeof(token));
    strcpy(head->name, "head");
    strcpy(head->type, "type");
    head->next = NULL;

    // split lex input file into tokens
    char * toks = strtok(lexInput, " ");

    // count tokens
    while(toks != NULL)
    {
      toks = strtok(NULL, " ");

      if (toks != NULL)
		{
	      insertTail(head, toks);
		}

    }

    printList(head);

    return;
}

void insertTail(token * head, char * name)
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

    // identify what type it's going to be
    strcpy(tmp->next->type, "test");
    tmp->next->next = NULL;
}

void printList(token * head)
{
    token * tmp = head->next;

    while(tmp != NULL)
    {
        printf("%s\t%s\n", tmp->name, tmp->type);
        tmp = tmp->next;
    }

   printf("END OF LIST\n");
}
