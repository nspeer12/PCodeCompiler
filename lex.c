// Written by Stephen Speer
// Written by Danish Siddiqui

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct lexeme
{
    char name[11];
    int type;

}lexeme;

lexeme masterArray[10000];

int getNumChars(FILE *fp);
void populateCharArray(char charArray[], int numOfChars, FILE *fp);
void cleanInput(int numOfChars, char charArray[]);
void evaluateTokens(char charArray[]);
int isALetter(char ch);
int isADigit(char ch);
int isSpecialWord(char temp[]);
void identify(char temp[], int ssym[], int *lexemesLength);
void printLexemeList(int *lexemesLength);
int isSpecialSymbol(char c);

typedef enum
{
  nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6, slashsym = 7, oddsym = 8,
  eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
  commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23,
  thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
  readsym = 32, elsesym = 33
}token_type;


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

int main(int argc, char *argv[])
{
    FILE *fp;
    int i =0;

    fp = fopen(argv[1],"r");
    int numOfChars = getNumChars(fp);
    fclose(fp);

    char charArray[numOfChars+2];

    fp = fopen(argv[1],"r");
    populateCharArray(charArray, numOfChars, fp);
    fclose(fp);

    // clears up all comments, \n and tabs.
    cleanInput(numOfChars, charArray);

    evaluateTokens(charArray);


}
void evaluateTokens(char charArray[])
{
  printf("Lexeme Table: \n");
  int lexemesLength = 0 ;
  int ssym[256];
  ssym['+'] = plussym; ssym['-'] = minussym; ssym['*'] = multsym;
  ssym['/'] = slashsym; ssym['('] = lparentsym; ssym[')'] = rparentsym;
  ssym['='] = eqlsym; ssym[','] = commasym; ssym['.'] = periodsym;
  ssym['#'] = neqsym; ssym['<'] = lessym; ssym['>'] = gtrsym;
  ssym['$'] = leqsym; ssym['%'] = geqsym; ssym[';'] = semicolonsym;

  printf("lexeme\ttoken type\n");

  int i = 0, z =0;
  char temp[15];
  for(z = 0; z < 15; z++)
      temp[z] = '\0';

  for( i = 0; i < strlen(charArray); i++)
  {
    char c = charArray[i];
    // get to a letter.
    if(c == ' ')
    {
      identify(temp,ssym,&lexemesLength);
      lexemesLength++;
    }
    else if(isSpecialSymbol(c))
    {
      if(strlen(temp) == 0)
      {
        if(c==':' && charArray[i+1]=='=')
        {
          temp[strlen(temp)] = c;
          temp[strlen(temp)] = charArray[++i];
          identify(temp,ssym,&lexemesLength);
          lexemesLength++;
        }
        else
        {
          temp[strlen(temp)] = c;
          identify(temp,ssym,&lexemesLength);
          lexemesLength++;
        }

      }
      else
      {
        identify(temp,ssym,&lexemesLength);
        lexemesLength++;
        i--;
      }
    }
    else
    {
      temp[strlen(temp)] = c;
    }
  }

  printLexemeList(&lexemesLength);
}
void cleanInput(int numOfChars, char charArray[])
{
   int i;

   // Removes tabs.
   for(i = 0; i < numOfChars; i++)
   {
     if(charArray[i]=='\t' || charArray[i]=='\n')
     {
       charArray[i] = ' ';
     }
   }

   // Removes comments.
   for(i = 0; i < numOfChars; i++)
   {
     // check for a comment sequence while iterating through char array.
     if(charArray[i] == '/' && i+1 < numOfChars && charArray[i+1] == '*')
     {
       charArray[i] = ' ';
       charArray[i+1] = ' ';
       i+=2;

       while(i < numOfChars)
       {
          if(charArray[i] == '*')
          {
             if(charArray[i+1] == '/')
             {
                charArray[i++] = ' ';
                charArray[i++] = ' ';
                break;
             }
             else
             {
                charArray[i++] = ' ';
                charArray[i++] = ' ';
             }
          }
          else
          {
             charArray[i++] = ' ';
          }
       }

     }

   }
}

void populateCharArray(char charArray[], int numOfChars, FILE *fp)
{
  char c;
  int i = 0;
  while(fscanf(fp,"%c",&c) != EOF)
    charArray[i++] = c;
}

int getNumChars(FILE *fp)
{
  char c;
  int i = 0;

  while(fscanf(fp,"%c",&c) != EOF)
    i++;

  return i;
}
int isADigit(char ch)
{
    int value = ch - '0';
    if(value >=0 && value < 10)
        return 1;
    return 0;
}
int isALetter(char ch)
{
    //checks lowercase
    int value = ch - 'a';
    if(value >= 0 && value < 26)
        return 1;

    //checks uppercase
    int value2 = ch - 'A';
    if(value2 >= 0 && value2 < 26)
        return 1;

    return 0;
}
int isSpecialWord(char temp[])
{
    int i;
    for(i = 0; i < 15; i++)
    {
        if(strcmp(temp, word[i]) == 0)
        {
            return wsym[i];
        }
    }
    return 0;
}
void identify(char temp[], int ssym[], int *lexemesLength)
{
  int z = 0;
  lexeme current;

  if(strlen(temp) > 0)
  {
    printf("%s\t",temp);

    // perform checks to see what temp qualifies as.
    // Does it qualify as a special word?
    if(isSpecialWord(temp) > 0)
    {
      printf("%d\n",isSpecialWord(temp));

      current.type = isSpecialWord(temp);
      strcpy(current.name,temp);
      masterArray[*lexemesLength] = current;

    }
    else if(isSpecialSymbol(temp[0]) > 0)
    {
      if(strlen(temp) > 1)
      {
        if(temp[1]=='=')
        {
          printf("%d\n",becomessym);

          current.type = becomessym;
          strcpy(current.name,temp);
          masterArray[*lexemesLength] = current;
        }
      }
      else
      {
        printf("%d\n",ssym[temp[0]]);

        current.type = ssym[temp[0]];
        strcpy(current.name,temp);
        masterArray[*lexemesLength] = current;
      }
    }
    else if(isALetter(temp[0]))
    {
      printf("%d\n",identsym);

      current.type = identsym;
      strcpy(current.name,temp);
      masterArray[*lexemesLength] = current;
    }
    else if(isADigit(temp[0]))
    {
      printf("%d\n", numbersym);
      current.type = numbersym;
      strcpy(current.name,temp);
      masterArray[*lexemesLength] = current;
    }
    else
    {
      printf("I'm NOT SURE!\n");
    }

    for(z = 0; z < 15; z++)
        temp[z] = '\0';
  }
}
int isSpecialSymbol(char c)
{
    int i;
    for(i=0; i < 13; i++)
    {
        if(c == symbols[i])
            return 1;
    }
    return 0;
}
void printLexemeList(int *lexemesLength)
{
  printf("Lexeme List: \n");

  int i = 0;

  for(int i = 0; i< *lexemesLength; i++)
  {
    printf("%d ",masterArray[i].type);
    if(masterArray[i].type == numbersym || masterArray[i].type == identsym)
    printf("%s ",masterArray[i].name);
  }

}
