#include "Scanner.h"

double thisNum;
int lenTokenChain, lenStatementM;


char* CharInput(char* filename)
{
    FILE *fp;
    fp = fopen(filename, "r");
    char *input = (char*)malloc(sizeof(char) * 1024);
    char ch = '0';
    int i = 0;
    while(ch != EOF)
    {
        ch = fgetc(fp);
        input[i] = ch;
        i++;
    }
    input[i - 1] = '\0';
    return input;
}

int thisWord(char* str)
{
    int index = 0;
    int lenMax = 0, indexTab = 0;
    char *word = (char*)malloc(sizeof(char) * 100);
    strncpy(word, str, strlen(str)>100?99:strlen(str));
    //printf("%s\n\n", word);
    for(int i = 0; i < 18; i++)
    {
        int match = 1;
        char *lex = (char*)malloc(sizeof(char) * 10);
        strcpy(lex, TokenTab[i].lexeme); 
        //printf("%s\n", lex);
        int lenLex = strlen(lex);
        for(int j = 0; j < lenLex; j++)
        {
            if(lex[j] != word[j])
            {
                match = 0;
                break;
            }
        }
        if(lenLex > lenMax && match == 1)
        {
            lenMax = lenLex;
            indexTab = i;
        }
    }
    return indexTab;
}

char* thisNumber(char* str)
{
    int index = 0;
    char ch = str[0];
    int l = 0, r = 0;
    double number = 0.0;
    char *doubleNum = (char*)malloc(sizeof(char) * 40);
    char *stack = (char*)malloc(sizeof(char) * 18);
    while(ch >= '0' && ch <= '9')
    {
        stack[l++] = ch;
        index++;
        ch = str[index];
    }
    double ten = 1.0;
    for(int i = l-1; i >= 0; i--)
    {
        number += ten * (stack[i] - 48);
        ten = ten * 10.0;
    }
    if(ch == '.')
    {
        ten = 0.1;
        index++;
        ch = str[index++];
        while(ch >= '0' && ch <= '9')
        {
            number += (ch - 48) * ten;
            ten /= 10.0;
            ch = str[index++];
        }
    }
    thisNum = number;
    strncpy(doubleNum, str, index);
    return doubleNum;
}

Token* CreateTokenStream(char* InputStream)
{
    Token *tokenChain = (Token*)malloc(sizeof(Token) * 100);
    int indexChain = 0;
    int indexStream = 0;
    int indexTab;
    int lenStream = strlen(InputStream);
    char ch;
    char *point;
    while(indexStream < lenStream)
    {
        ch = InputStream[indexStream];
        if(ch == ' ' || ch == '\t')
        {
            indexStream++;
            continue;
        }
        if(ch == '\n')
        {
            indexStream++;
            continue;
        }
        if(ch >= 65 && ch <=122)
        {
            point = &InputStream[indexStream];
            indexTab = thisWord(point);
            tokenChain[indexChain] = TokenTab[indexTab];
            indexChain++;
            indexStream += strlen(TokenTab[indexTab].lexeme);
            //printf("%s %d %s\n", point, indexTab, TokenTab[indexTab].lexeme);
            continue;
        }
        if(ch >= '0' && ch <= '9')
        {
            point = &InputStream[indexStream];
            char* doubleNum = thisNumber(point);
            int lenNum = strlen(doubleNum);
            Token tokenNum = {CONST_ID, doubleNum, thisNum,  NULL};
            tokenChain[indexChain] = tokenNum;
            indexChain++;
            indexStream += lenNum;
            continue;
        }
        if(ch == '*')
        {
            if(InputStream[indexStream + 1] == '*')
            {
                tokenChain[indexChain] = {POWER, "**", 0.0, NULL};
                indexChain++;
                indexStream += 2;
            }else{
                tokenChain[indexChain] = {MUL, "*", 0.0, NULL};
                indexChain++;
                indexStream++;
            }
            continue;
        }
        if(ch == '/')
        {
            if(InputStream[indexStream + 1] == '/')
            {
                indexStream += 2;
            }else{
                tokenChain[indexChain] = {DIV, "/", 0.0, NULL};
                indexChain++;
                indexStream++;
            }
            continue;
        }
        if(ch == '-')
        {
            if(InputStream[indexStream + 1] == '-')
            {
                indexStream += 2;
            }else{
                tokenChain[indexChain] = {MINUS, "-", 0.0, NULL};
                indexChain++;
                indexStream++;
            }
            continue;
        }
        switch(ch)
        {
            case ';': tokenChain[indexChain] = {SEMICO, ";", 0.0, NULL};
                break;
            case '(': tokenChain[indexChain] = {L_BRACKET, "(", 0.0, NULL};
                break;
            case ')': tokenChain[indexChain] = {R_BRACKET, ")", 0.0, NULL};
                break;
            case ',': tokenChain[indexChain] = {COMMA, ",", 0.0, NULL};
                break;
            case '+': tokenChain[indexChain] = {PLUS, "+", 0.0, NULL};
                break;
        }
        indexChain++;
        indexStream++;
    }
    lenTokenChain = indexChain;
    return tokenChain;
}

struct Token* Scanner(char* filename)
{
    char *InputStream = CharInput(filename);
    Token* tokenChain = CreateTokenStream(InputStream);
/*
    printf("  type    lexeme       value      function\n");
    printf("----------------------------------------------------------\n");
    for(int i = 0; i <lenTokenChain; i++)
    {
        printf("%6d %10s %12f %10x\n", tokenChain[i].type, tokenChain[i].lexeme, tokenChain[i].value, tokenChain[i].FuncPtr);
    }*/

    return tokenChain;
}
/*
int main(int argc, char* argv[])
{
    Scanner(argv[1]);
    return 0;
}*/
