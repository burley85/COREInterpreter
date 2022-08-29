#include "Interpreter.h"
#include <stdlib.h>

void execDeclSeq(DeclSeq *d, Memory *mem);
void execDecl(Decl *d, Memory *mem);
void execDeclInt(DeclInt *d, Memory *mem);
void execDeclRef(DeclRef *d, Memory *mem);
void execIDList(IDList *i, Memory *mem);
void execStmtSeq(StmtSeq *s, Memory *mem, FILE *input);
void execStmt(Stmt *s, Memory *mem, FILE *input);
void execIf(If *i, Memory *mem, FILE *input);
void execLoop(Loop *l, Memory *mem, FILE *input);
int execCond(Cond *c, Memory *mem);
int execCmpr(Cmpr *c, Memory *mem);
void execAssign(Assign *a, Memory *mem, FILE *input);
int execExpr(Expr *e, Memory *mem);
int execTerm(Term *t, Memory *mem);
int execFactor(Factor *f, Memory *mem);
void execOut(Out *o, Memory *mem);
void execFuncDecl(FuncDecl *f, Memory *mem);
void execFormals(Formals *actualParams, Formals *formalParams, Memory *mem);
void execFuncCall(FuncCall *f, Memory *mem, FILE *input);

void execProg(Prog *p, Memory *mem, FILE *input){
    if(p->declSeq) execDeclSeq(p->declSeq, mem);
    enterFrame(mem);
    enterScope(mem);
    execStmtSeq(p->stmtSeq, mem, input);
    exitScope(mem);
    exitFrame(mem);
}

void execDeclSeq(DeclSeq *d, Memory *mem){
    if(d->decl) execDecl(d->decl, mem);
    if(d->funcDecl) execFuncDecl(d->funcDecl, mem);
    if(d->declSeq) execDeclSeq(d->declSeq, mem);
}

void execDecl(Decl *d, Memory *mem){
    if(d->declInt) execDeclInt(d->declInt, mem);
    if(d->declRef) execDeclRef(d->declRef, mem);
}

void execDeclInt(DeclInt *d, Memory *mem){
    execIDList(d->idList, mem);
}

void execDeclRef(DeclRef *d, Memory *mem){
    execIDList(d->idList, mem);
}

void execIDList(IDList *i, Memory *mem){
    declareVariable(mem, i->id->name, i->id->type);
    if(i->idList) execIDList(i->idList, mem);
}

void execStmtSeq(StmtSeq *s, Memory *mem, FILE *input){
    execStmt(s->stmt, mem, input);
    if(s->stmtSeq) execStmtSeq(s->stmtSeq, mem, input);
}

void execStmt(Stmt *s, Memory *mem, FILE *input){
    if(s->assign) execAssign(s->assign, mem, input);
    if(s->decl) execDecl(s->decl, mem);
    if(s->funcCall) execFuncCall(s->funcCall, mem, input);
    if(s->ifStmt) execIf(s->ifStmt, mem, input);
    if(s->loop) execLoop(s->loop, mem, input);
    if(s->out) execOut(s->out, mem);
}

void execIf(If *i, Memory *mem, FILE *input){
    if(execCond(i->cond, mem)){
        enterScope(mem);
        execStmtSeq(i->ifStmtSeq, mem, input);
        exitScope(mem);
    }
    else if(i->elseStmtSeq){
        enterScope(mem);
        execStmtSeq(i->elseStmtSeq, mem, input);
        exitScope(mem);
    }
}

void execLoop(Loop *l, Memory *mem, FILE *input){
    while(execCond(l->cond, mem)){
        enterScope(mem);
        execStmtSeq(l->stmtSeq, mem, input);
        exitScope(mem);
    }
}

int execCond(Cond *c, Memory *mem){
    if(c->cmpr && c->cond) return (execCmpr(c->cmpr, mem) || execCond(c->cond, mem));
    if(c->cmpr) return execCmpr(c->cmpr, mem);
    if(c->cond) return !execCond(c->cond, mem);
}

int execCmpr(Cmpr *c, Memory *mem){
    int v1 = execExpr(c->expr1, mem);
    int v2 = execExpr(c->expr2, mem);
    switch (c->operator)
    {
    case EQUAL:
        return v1 == v2;
    case LESS:
        return v1 < v2;
    case LESSEQUAL:
        return v1 <= v2;
    default:
        printf("ERROR: invalid value of operator for Cmpr.\n");
        exit(1);
    }
    
}

void execAssign(Assign *a, Memory *mem, FILE *input){
    if(a->inputAssignment){
        int val;
        if (!fscanf(input, "%d", &val)){
            printf("ERROR: could not read input from input file.\n");
            exit(1);
        }
        assign(mem, a->id->name, val);
    }
    else if(a->expr) assign(mem, a->id->name, execExpr(a->expr, mem));
    else if(a->id2) share(mem, a->id->name, a->id2->name, 0);
    else newClass(mem, a->id->name);
}

int execExpr(Expr *e, Memory *mem){
    int t = execTerm(e->term, mem);
    if(!e->expr) return t;
    if(e->addOperator) return t + execExpr(e->expr, mem);
    return t - execExpr(e->expr, mem);

}

int execTerm(Term *t, Memory *mem){
    if(!t->term) return execFactor(t->factor, mem);
    return execFactor(t->factor, mem) * execTerm(t->term, mem);
}

int execFactor(Factor *f, Memory *mem){
    if(f->expr) return execExpr(f->expr, mem);
    if(f->id) return getValue(mem, f->id->name);
    return f->constVal;
}

void execOut(Out *o, Memory *mem){
    printf("%d\n", execExpr(o->expr, mem));
}

void execFuncDecl(FuncDecl *f, Memory *mem){
    declareFunction(mem, f);
}

void execFormals(Formals *actualParams, Formals *formalParams, Memory *mem){
    passParameters(mem, actualParams, formalParams);
}

void execFuncCall(FuncCall *f, Memory *mem, FILE *input){
    FuncDecl *funcDecl = getFuncDecl(mem, f->id->name);
    enterFrame(mem);
    enterScope(mem);
    execFormals(f->formals, funcDecl->formals, mem);
    execStmtSeq(funcDecl->stmtSeq, mem, input);
    exitScope(mem);
    exitFrame(mem);
}