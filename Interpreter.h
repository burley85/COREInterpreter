#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "StructDefs.h"
#include "Memory.h"

void execProg(Prog *prog, Memory *mem, FILE *input);

#endif