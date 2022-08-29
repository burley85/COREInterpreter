#include "memory.h"
#include <stdlib.h>
#include <string.h>

#define DEBUG 0

Memory *initMemory(){
    if(DEBUG) printf("DEBUG: entering %s\n", __func__);
    Memory *mem = malloc(sizeof(Memory));
    
    mem->heapSize = 0;
    mem->maxHeapSize = 0;
    mem->heap = NULL;

    mem->maxStaticMemSize = 0;
    mem->staticMemSize = 0;
    mem->staticMem = NULL;

    mem->stackSize = 0;
    mem->maxStackSize = 0;
    mem->stack = NULL;

    mem->funcDeclArray = NULL;
    mem->maxFuncDeclArraySize = 0;
    mem->funcDeclArraySize = 0;

    return mem;
}

void enterFrame(Memory *mem){
    if(DEBUG) printf("DEBUG: entering %s\n", __func__);
    if(mem->stackSize == mem->maxStackSize){
        if(DEBUG) printf("DEBUG: enterFrame: expanding maxStackSize from %d", mem->maxStackSize);
        mem->maxStackSize = (mem->maxStackSize * 2) + 1;
        if(DEBUG) printf(" to %d.\n", mem->maxStackSize);
        mem->stack = realloc(mem->stack, mem->maxStackSize * sizeof(StackFrame));
        if(!mem->stack){
            printf("ERROR: realloc in enterFrame failed.\n");
            exit(1);
        }
        if(DEBUG) printf("DEBUG: realloc success.\n");
    }
    mem->stack[mem->stackSize].stackFrame = NULL;
    mem->stack[mem->stackSize].stackFrameSize = 0;
    mem->stack[mem->stackSize].maxStackFrameSize = 0;
    mem->stackSize++;
}

void exitFrame(Memory *mem){
    if(DEBUG) printf("DEBUG: entering %s\n", __func__);
    if (mem->stackSize == 0){
        printf("ERROR: exitFrame called on an empty stack.\n", mem->stack[mem->stackSize - 1].stackFrameSize);
        exit(1);
    }
    if (mem->stack[mem->stackSize - 1].stackFrameSize){
        printf("ERROR: exitFrame called before exiting all scopes. Frame size: %d.\n", mem->stack[mem->stackSize - 1].stackFrameSize);
        exit(1);
    }
    mem->stackSize--;
}

void enterScope(Memory *mem){
    if(DEBUG) printf("DEBUG: entering %s\n", __func__);
    if (mem->stackSize == 0){
        printf("ERROR: enterScope called on an empty stack.\n", mem->stack[mem->stackSize - 1].stackFrameSize);
        exit(1);
    }
    StackFrame* currentFrame = &(mem->stack[mem->stackSize - 1]);
    if(currentFrame->maxStackFrameSize == currentFrame->stackFrameSize){
        if(DEBUG) printf("DEBUG: expanding maxStackFrameSize from %d", currentFrame->maxStackFrameSize);
        currentFrame->maxStackFrameSize = (currentFrame->maxStackFrameSize * 2) + 1;
        if(DEBUG) printf(" to %d.\n", currentFrame->maxStackFrameSize);
        currentFrame->stackFrame = realloc(currentFrame->stackFrame, currentFrame->maxStackFrameSize * sizeof(Scope));
        if(!currentFrame->stackFrame){
            printf("ERROR: realloc in enterScope failed.\n");
            exit(1);
        }
        if(DEBUG) printf("DEBUG: realloc success.\n");
    }
    currentFrame->stackFrame[currentFrame->stackFrameSize].scope = NULL;
    currentFrame->stackFrame[currentFrame->stackFrameSize].scopeSize = 0;
    currentFrame->stackFrame[currentFrame->stackFrameSize].maxScopeSize = 0;
    currentFrame->stackFrameSize++;
}

void exitScope(Memory *mem){
    if(DEBUG) printf("DEBUG: entering %s\n", __func__);
    if (mem->stackSize == 0){
        printf("ERROR: exitScope called on an empty stack.\n", mem->stack[mem->stackSize - 1].stackFrameSize);
        exit(1);
    }
    StackFrame* currentFrame = &(mem->stack[mem->stackSize - 1]);
    if (currentFrame->stackFrameSize == 0){
        printf("ERROR: exitScope called on an empty frame.\n", mem->stack[mem->stackSize - 1].stackFrameSize);
        exit(1);
    }
    currentFrame->stackFrameSize--;
}

Binding *findBinding(Memory *mem, char *idName){
    if(DEBUG) printf("DEBUG: entering %s, looking for %s\n", __func__, idName);
    Binding *b = NULL;
    //Search current frame(last scope first)
    if (mem->stackSize > 0){
        if(DEBUG) printf("DEBUG: searching stack frame %d\n", mem->stackSize);
        StackFrame* currentFrame = &(mem->stack[mem->stackSize - 1]);
        int i = currentFrame->stackFrameSize - 1;
        while(i >= 0 && !b){
            // search currentFrame->stackFrame[i]
            if(DEBUG) printf("DEBUG: searching scope %d\n", i + 1);
            Scope* s = &(currentFrame->stackFrame[i]);
            int j = 0;
            while(j < s->scopeSize && !b) {
                if(!strcmp(s->scope[j].id.name, idName)) b = &(s->scope[j]);
                j++;
            }
            i--;
        }
    }
    //Search staticMem
    if(!b){
        if(DEBUG) printf("DEBUG: searching static memory\n");
        int i = 0;
        while(i < mem->staticMemSize && !b){
            if(!strcmp(mem->staticMem[i].id.name, idName)) b = &(mem->staticMem[i]);
            i++;
        }
    }
    return b;
}

int getValue(Memory *mem, char *idName){
    if(DEBUG) printf("DEBUG: entering %s\n", __func__);
    Binding *b = findBinding(mem, idName);
    if(!b){
        printf("ERROR: getValue could not find ID, %s.\n", idName);
        exit(0);
    }
    if(b->id.type == RefID) return mem->heap[b->value];
    if(b->id.type == IntID) return b->value;
}

void assign(Memory *mem, char *idName, int val){
    if(DEBUG) printf("DEBUG: entering %s\n", __func__);
    Binding *b = findBinding(mem, idName);
    if(!b){
        printf("ERROR: assign could not find ID, %s.\n", idName);
        exit(0);
    }
    if(b->id.type == RefID) mem->heap[b->value] = val;
    if(b->id.type == IntID) b->value = val;
}

void share(Memory *mem, char *dest, char *src, int address){
    if(DEBUG) printf("DEBUG: entering %s\n", __func__);
    
    Binding *bDest = findBinding(mem, dest);
    if(!bDest){
        printf("ERROR: assign could not find ID, %s.\n", dest);
        exit(0);
    }

    if(src) {
        Binding *bSrc = findBinding(mem, src);
        if(!bSrc){
            printf("ERROR: assign could not find ID, %s.\n", src);
            exit(0);
        }
        bDest->value = bSrc->value;
    }
    else{
        bDest->value = address;
    }
}

void newClass(Memory *mem, char *idName){
    if(DEBUG) printf("DEBUG: entering %s\n", __func__);
    Binding *b = findBinding(mem, idName);
    if(mem->heapSize == mem->maxHeapSize){
        if(DEBUG) printf("DEBUG: expanding maxHeapSize from %d", mem->maxHeapSize);
        mem->maxHeapSize = (mem->heapSize * 2) + 1;
        if(DEBUG) printf(" to %d.\n", mem->maxHeapSize);
        mem->heap = realloc(mem->heap, mem->maxHeapSize * sizeof(int));
        if(!mem->heap){
            printf("ERROR: realloc in enterScope failed.\n");
            exit(1);
        }
        if(DEBUG) printf("DEBUG: realloc success.\n");
    }
    mem->heap[mem->heapSize] = 0;
    b->value = mem->heapSize;
    mem->heapSize++;
}

void declareVariable(Memory *mem, char *idName, idType type){
    if(DEBUG) printf("DEBUG: entering %s, declaring %s\n", __func__, idName);
    Binding b;
    b.id.name = malloc(MAX_ID_LENGTH * sizeof(char));
    strcpy(b.id.name, idName);
    b.id.type = type;
    b.value = 0;

    if(mem->stackSize > 0){
        if(DEBUG) printf("DEBUG: declaring variable %s in the stack.\n", idName);
        StackFrame* currentFrame = &(mem->stack[mem->stackSize - 1]);
        Scope *currentScope = &(currentFrame->stackFrame[currentFrame->stackFrameSize - 1]);
        if(currentScope->scopeSize == currentScope->maxScopeSize){
            if(DEBUG) printf("DEBUG: expanding maxScopeSize from %d", currentScope->maxScopeSize);
            currentScope->maxScopeSize = (currentScope->maxScopeSize * 2) + 1;
            if(DEBUG) printf(" to %d.\n", currentScope->maxScopeSize);
            currentScope->scope = realloc(currentScope->scope, currentScope->maxScopeSize * sizeof(Binding));
            if(!currentScope->scope){
                printf("ERROR: realloc in declareVariable failed.\n");
                exit(1);
            }
            if(DEBUG) printf("DEBUG: realloc success.\n");
        }
        currentScope->scope[currentScope->scopeSize] = b;
        currentScope->scopeSize++;
    }
    
    else{
        if(DEBUG) printf("DEBUG: declaring variable %s in the heap.\n", idName);
        if(mem->maxStaticMemSize == mem->staticMemSize){
            if(DEBUG) printf("DEBUG: expanding maxStaticMemSize from %d", mem->maxStaticMemSize);
            mem->maxStaticMemSize = (mem->maxStaticMemSize * 2) + 1;
            if(DEBUG) printf(" to %d.\n", mem->maxStaticMemSize);
            mem->staticMem = realloc(mem->staticMem, mem->maxStaticMemSize * sizeof(Binding));
            if(!mem->staticMem){
                printf("ERROR: realloc in declareVariable failed.\n");
                exit(1);
            }
            if(DEBUG) printf("DEBUG: realloc success.\n");
        }
        mem->staticMem[mem->staticMemSize] = b;
        mem->staticMemSize++;
    }
}

void declareFunction(Memory *mem, FuncDecl *funcDecl){
    if(DEBUG) printf("DEBUG: entering %s\n", __func__);
    if(mem->funcDeclArraySize == mem->maxFuncDeclArraySize){
        if(DEBUG) printf("DEBUG: declareFunction: expanding maxFuncDeclArraySize from %d", mem->maxFuncDeclArraySize);
        mem->maxFuncDeclArraySize = (mem->maxFuncDeclArraySize * 2) + 1;
        if(DEBUG) printf(" to %d.\n", mem->maxFuncDeclArraySize);
        mem->funcDeclArray = realloc(mem->funcDeclArray, mem->maxFuncDeclArraySize * sizeof(FuncDecl *));
        if(!mem->funcDeclArray){
            printf("ERROR: realloc in declareFunction failed.\n");
            exit(1);
        }
        if(DEBUG) printf("DEBUG: realloc success.\n");
    }
    mem->funcDeclArray[mem->funcDeclArraySize] = funcDecl;
    mem->funcDeclArraySize++;
}

FuncDecl *getFuncDecl(Memory *mem, char *idName){
    if(DEBUG) printf("DEBUG: entering %s\n", __func__);
    int i = mem->funcDeclArraySize - 1;
    while(i >= 0){
        if(!strcmp(mem->funcDeclArray[i]->id->name, idName)) return mem->funcDeclArray[i];
        i--;
    }
}

void passParameters(Memory *mem, Formals *actual, Formals *formal){
    if(DEBUG) printf("DEBUG: entering %s\n", __func__);
    mem->stackSize--; //Ensures findBinding searches the previous stack frame, rather than the current
    int actualParamAddress = findBinding(mem, actual->id->name)->value;
    mem->stackSize++;
    declareVariable(mem, formal->id->name, RefID);
    share(mem, formal->id->name, NULL, actualParamAddress);
    if(actual->formals && formal->formals) passParameters(mem, actual->formals, formal->formals);
}