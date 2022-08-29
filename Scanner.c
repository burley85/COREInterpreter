#include <stdio.h>
#include "StructDefs.h"
#include "Scanner.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char* tokenToString(tokenType token){
    char *tokenTypeString[] = {"PROGRAM", "BEGIN", "END", "NEW", "DEFINE", "EXTENDS",
        "CLASS", "INT", "IF", "THEN", "ELSE", "WHILE", "ENDIF", "SEMICOLON", "LPAREN",
        "RPAREN", "LBRACE", "RBRACE", "COMMA", "ASSIGN", "NEGATION", "OR", "EQUAL",
        "LESS", "LESSEQUAL", "ADD", "SUB", "MULT", "INPUT", "OUTPUT", "REF", "SHARE",
        "CONST", "ID", "EOS", "ERROR", "OTHER"
    };
    return tokenTypeString[token];
}

int getCONST(Scanner *scanner){
    return scanner->currentValue;
}

char* getID(Scanner *scanner){
    return scanner->currentID;
}

tokenType currentToken(Scanner *scanner){
    return scanner->currentToken;
}

int peek(FILE *stream)
{
    int c;
    c = fgetc(stream);
    ungetc(c, stream);
    return c;
}

static tokenType checkSpecialCharacter(Scanner *scanner, char c){
    char nextLetter;
    switch(c){
        case ';':
            return SEMICOLON;
        case '(':
			return LPAREN;
		case ')':
			return RPAREN;
		case ',':
			return COMMA;
		case '!':
			return NEGATION;
		case '+':
			return ADD;
		case '-':
			return SUB;
		case '*':
			return MULT;
		case '{':
			return LBRACE;
		case '}':
			return RBRACE;
		case '=':
            nextLetter = fgetc(scanner->file);
            if(nextLetter == '='){
                return EQUAL;
            }
            fseek(scanner->file, -1, SEEK_CUR);
			return ASSIGN;
		case '<':
            nextLetter = fgetc(scanner->file);
            if(nextLetter == '='){
                return LESSEQUAL;
            }
            fseek(scanner->file, -1, SEEK_CUR);
			return LESS;
        default:
            return OTHER;
    }    
}

// Pushes some characters from str back onto input stream starting at startPos
static void ungetWord(Scanner *scanner, char* str, int startPos, int lettersToUnget){
    if(lettersToUnget){
        char c = str[startPos + lettersToUnget - 1];
        while(lettersToUnget > 0){
            ungetc(c, scanner->file);
            lettersToUnget--;
            c = str[startPos + lettersToUnget - 1];
        }
    }
}

static tokenType checkKeyword(Scanner *scanner, char c){
    char word[9];
    char *keywords[] = {"program", "begin", "end", "new", "int", "define", "class",
    "extends", "if", "then", "else", "while", "or", "input", "output", "ref", "share"};
    tokenType correspondingTokens[] = {PROGRAM, BEGIN, END, NEW, INT, DEFINE, CLASS,
    EXTENDS, IF, THEN, ELSE, WHILE, OR, INPUT, OUTPUT, REF, SHARE};
    fscanf(scanner->file,"%8[a-zA-z0-9]", &word);

    for(int i = 0; i < 17; i++){
        if(!strcmp(word, keywords[i])) return correspondingTokens[i];
    }
    return OTHER;
}


static tokenType checkConstant(Scanner *scanner, char c){
    if (isdigit(c)){
        fscanf(scanner->file, "%d", &(scanner->currentValue));
        if (scanner->currentValue > MAX_INT){
            printf("ERROR: %d is too large to be a valid constant.\n", scanner->currentValue);
            exit(1);
        }
        return CONST;
    }
    return OTHER;
}

static tokenType checkIdentifier(Scanner *scanner, char c){
    int maxLen = MAX_ID_LENGTH;
    if (isalpha(c)){
        char str[64];
        fscanf(scanner->file, "%32[a-zA-Z0-9]", &str);
        strcpy(scanner->currentID, str);
        if(scanner->currentID[MAX_ID_LENGTH - 1] != '\0'){
            printf("ERROR: ID %s is too long.\n", scanner->currentID);
            exit(1);
        }
        return IDENTIFIER;
    }
    return ERROR;
}

tokenType nextToken(Scanner *scanner){
    tokenType token;
    int startPos;
    char nextLetter = ' ';
    scanner->currentValue = 0;
    memset(scanner->currentID, 0, sizeof(scanner->currentID));

    //Remove leading whitespace
    while((nextLetter == ' ' || nextLetter == '\t' || nextLetter == '\n' || nextLetter == '\0' || nextLetter < 0) && !feof(scanner->file)){
        fscanf(scanner->file, "%1s", &nextLetter);
    }
    startPos = ftell(scanner->file) - 1;
    
    //Check for EOS
    if(feof(scanner->file)) token = EOS;

    //Check for special characters
    else token = checkSpecialCharacter(scanner, nextLetter);
    
    //Check for keyword
    if(token == OTHER){
        fseek(scanner->file, startPos, SEEK_SET);   
        token = checkKeyword(scanner, nextLetter);
    }

    //Check for constant
    if(token == OTHER){
        fseek(scanner->file, startPos, SEEK_SET);   
        token = checkConstant(scanner, nextLetter);
    }

    //Check for identifier
    if(token == OTHER){
        fseek(scanner->file, startPos, SEEK_SET);   
        token = checkIdentifier(scanner, nextLetter);
    }
    if(token == ERROR){
        printf("ERROR: Invalid characer: '%c'\n", nextLetter);
        exit(1);
    }

    scanner->currentToken = token;
    return token;
}

tokenType initScanner(char *fileName, Scanner *scanner){
    scanner->file = fopen(fileName,"rb");
    if(errno) printf("Error opening file %s\n", fileName);
    return nextToken(scanner);
}
