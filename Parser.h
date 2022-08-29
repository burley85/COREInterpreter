#ifndef PARSER_H
#define PARSER_H

typedef struct Parser{
    Prog *prog;
} Parser;

typedef struct IDListStack{
    int stackSize;
    IDList *stack[MAX_SCOPE_NESTING];
} IDListStack;

Prog *parseProg(Scanner *scanner);

#endif