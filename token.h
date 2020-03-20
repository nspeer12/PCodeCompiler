#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct token
{
    int type;
    char * name;
    char * value;
    struct token * next;

} token;


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

typedef enum
{
	nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6, slashsym = 7, oddsym = 8,
	eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
	commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23,
	thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
	readsym = 32, elsesym = 33

}token_type;

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
