#include "StructDefs.h"
#include "Scanner.h"
#include "Parser.h"
#include "ParsePrinter.h"
#include "Interpreter.h"
#include <stdlib.h>
#include <string.h>

int filecmp(char* s1, char* s2){
    if(errno) printf("Error opening file %s\n", " asdasdsad");
    FILE *f1 = fopen(s1, "r");
    if(errno) printf("Error opening file %s\n", s1);
    FILE *f2 = fopen(s2, "r");
    printf("Filecmp(%s, %s)\n", s1, s2);
    char c1, c2;
    fscanf(f1, "%c", &c1);
    fscanf(f2, "%c", &c2);
    while (c1 != EOF && c2 != EOF && !feof(f1) && !feof(f2)) {
        if (c1 != c2){
            printf("%c != %c\n", c1, c2);
            return 0;
        }
        fscanf(f1, "%c", &c1);
        fscanf(f2, "%c", &c2);
    }
    if (c1 != c2){
        printf("%c != %c\n", c1, c2);
        return 0;
    }
    return 1;
}


int main(int argc, char **argv){
    Scanner s;
    Prog *p;
    Memory *m = initMemory();
    FILE *input;
    
    if(argc > 1) initScanner(argv[1], &s);
    else initScanner("./0.code", &s);

    p = parseProg(&s);
    //printProg(p, 0);
    
    if(argc > 2) input = fopen(argv[2], "r");
    else input = stdin;
    execProg(p, m, input);

    return 0;
}
