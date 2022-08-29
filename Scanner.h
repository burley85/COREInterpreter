#ifndef SCANNER_H
#define SCANNER_H

#include "StructDefs.h"

char* tokenToString(tokenType token);
tokenType initScanner(char *fileName, Scanner *scanner);
tokenType nextToken(Scanner *scanner);
tokenType currentToken(Scanner *scanner);
char* getID(Scanner *scanner);
int getCONST(Scanner *scanner);

#endif