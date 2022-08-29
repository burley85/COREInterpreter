#ifndef MEMORY_H
#define MEMORY_H
#include "StructDefs.h"



typedef struct Memory
{
    int *heap;
    int heapSize;
    int maxHeapSize;

    struct StackFrame *stack;
    int stackSize;
    int maxStackSize;

    struct Binding *staticMem;
    int staticMemSize;
    int maxStaticMemSize;

    FuncDecl **funcDeclArray;
    int funcDeclArraySize;
    int maxFuncDeclArraySize;
} Memory;

typedef struct StackFrame
{
    struct Scope *stackFrame;
    int stackFrameSize;
    int maxStackFrameSize;
} StackFrame;

typedef struct Scope
{
    struct Binding *scope;
    int scopeSize;
    int maxScopeSize;
} Scope;

typedef struct Binding
{
    ID id;
    int value;
} Binding;

Memory *initMemory();
int getValue(Memory *mem, char *idName);
void assign(Memory *mem, char *idName, int val);
void share(Memory *mem, char *dest, char *src, int address);
Binding *searchMem(Memory *mem, char *idName);
void newClass(Memory *mem, char *idName);
void enterScope(Memory *mem);
void exitScope(Memory *mem);
void enterFrame(Memory *mem);
void exitFrame(Memory *mem);
void declareVariable(Memory *mem, char *idName, idType type);
void declareFunction(Memory *mem, FuncDecl *funcDecl);
FuncDecl *getFuncDecl(Memory *mem, char *idName);
void passParameters(Memory *mem, Formals *actual, Formals *formal);

#endif