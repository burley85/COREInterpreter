#include "ParsePrinter.h"
#include <string.h>


// Function declarations
//----------------------------------------------------------------

void printDeclSeq(DeclSeq *declSeq, int indent);
void printDecl(Decl *decl, int indent);
void printDeclInt(DeclInt *declInt, int indent);
void printDeclRef(DeclRef *declRef, int indent);
void printIDList(IDList *declList, int indent);
void printStmtSeq(StmtSeq *stmtSeq, int indent);
void printStmt(Stmt *stmt, int indent);
void printAssign(Assign *assign, int indent);
void printIf(If *ifStmt, int indent);
void printLoop(Loop *loop, int indent);
void printCond(Cond *cond, int indent);
void printCmpr(Cmpr *cmpr, int indent);
void printExpr(Expr *expr, int indent);
void printTerm(Term *term, int indent);
void printFactor(Factor *factor, int indent);
void printOut(Out *out, int indent);
void printFuncDecl(FuncDecl *funcDecl, int indent);
void printFormals(Formals *formals, int indent);
void printFuncCall(FuncCall *funcCall, int indent);


void printIndents(int indents);

//----------------------------------------------------------------

void printProg(Prog *p, int indent){
    printIndents(indent);
    printf("program\n");

    if(p->declSeq) printDeclSeq(p->declSeq, indent + 1);

    printIndents(indent);
    printf("begin\n");

    printStmtSeq(p->stmtSeq, indent + 1);

    printIndents(indent);
    printf("end\n");
}

void printDeclSeq(DeclSeq *declSeq, int indent){
    if(declSeq->decl) printDecl(declSeq->decl, indent);
    else printFuncDecl(declSeq->funcDecl, indent);
    if(declSeq->declSeq) printDeclSeq(declSeq->declSeq, indent);
}

void printDecl(Decl *decl, int indent){
    if (decl->declInt) printDeclInt(decl->declInt, indent);
    else printDeclRef(decl->declRef, indent);
}

void printDeclInt(DeclInt *declInt, int indent){
    printIndents(indent);
    printf("int ");

    printIDList(declInt->idList, indent);

    printf(";\n");
}

void printDeclRef(DeclRef *declInt, int indent){
    printIndents(indent);
    printf("ref ");

    printIDList(declInt->idList, indent);

    printf(";\n");
}

void printIDList(IDList *idList, int indent){
    printf(idList->id->name);
    if(idList->idList){
        printf(",");
        printIDList(idList->idList, indent);
    }
}

void printStmtSeq(StmtSeq *stmtSeq, int indent){
    printStmt(stmtSeq->stmt, indent);
    if(stmtSeq->stmtSeq) printStmtSeq(stmtSeq->stmtSeq, indent);
}

void printStmt(Stmt *stmt, int indent){
    if(stmt->assign) printAssign(stmt->assign, indent);
    if(stmt->decl) printDecl(stmt->decl, indent);
    if(stmt->funcCall) printFuncCall(stmt->funcCall, indent);
    if(stmt->ifStmt) printIf(stmt->ifStmt, indent);
    if(stmt->loop) printLoop(stmt->loop, indent);
    if(stmt->out) printOut(stmt->out, indent);
}

void printAssign(Assign *assign, int indent){
    printIndents(indent);
    printf("%s = ", assign->id->name);
    if(assign->inputAssignment) printf("input()");
    else if(assign->expr) printExpr(assign->expr, indent);
    else if(assign->id2) printf("share %s", assign->id2->name);
    else printf(" new class");
    printf(";\n");
}

void printIf(If *ifStmt, int indent){
    printIndents(indent);
    printf("if ");
    printCond(ifStmt->cond, indent);
    printf(" then {\n");
    printStmtSeq(ifStmt->ifStmtSeq, indent + 1);
    if(ifStmt->elseStmtSeq){
        printIndents(indent);
        printf("} else {\n");
        printStmtSeq(ifStmt->elseStmtSeq, indent + 1);
    }
    printIndents(indent);
    printf("}\n");
}

void printLoop(Loop *loop, int indent){
    printIndents(indent);
    printf("while ");
    printCond(loop->cond, indent);
    printf(" {\n");
    printStmtSeq(loop->stmtSeq, indent + 1);
    printIndents(indent);
    printf("}\n");
}

void printCond(Cond *cond, int indent){
    if(cond->cmpr){
        printCmpr(cond->cmpr, indent);
        if(cond->cond){
            printf(" or ");
            printCond(cond->cond, indent);
        }
    }
    else{
        printf("!(");
        printCond(cond->cond, indent);
        printf(")");
    }
}

void printCmpr(Cmpr *cmpr, int indent){
    printExpr(cmpr->expr1, indent);
    switch (cmpr->operator)
    {
    case EQUAL:
        printf(" == ");
        break;
    case LESS:
        printf(" < ");
        break;
    case LESSEQUAL:
        printf(" <= ");
        break;
    default:
        printf("ERROR: Unexpected operator type operator.\n");
        break;
    }
    printExpr(cmpr->expr2, indent);
}

void printExpr(Expr *expr, int indent){
    printTerm(expr->term, indent);
    if(expr->expr){
        if(expr->addOperator) printf(" + ");
        else printf(" - ");
        printExpr(expr->expr, indent);
    }
}

void printTerm(Term *term, int indent){
    printFactor(term->factor, indent);
    if(term->term){
        printf(" * ");
        printTerm(term->term, indent);
    }
}

void printFactor(Factor *factor, int indent){
    if(factor->id) printf("%s", factor->id->name);
    else if(factor->expr){
        printf("(");
        printExpr(factor->expr, indent);
        printf(")");
    }
    else printf("%d", factor->constVal);
}

void printOut(Out *out, int indent){
    printIndents(indent);
    printf("output(");
    printExpr(out->expr, indent);
    printf(");\n");
}

void printFuncDecl(FuncDecl *funcDecl, int indent){
    printIndents(indent);
    printf("%s(ref ", funcDecl->id->name);
    printFormals(funcDecl->formals, indent);
    printf(") {\n");
    printStmtSeq(funcDecl->stmtSeq, indent + 1);
    printf("}\n");
}

void printFormals(Formals *formals, int indent){
    printf(formals->id->name);
    if(formals->formals){
        printf(",");
        printFormals(formals->formals, indent);
    }
}

void printFuncCall(FuncCall *funcCall, int indent){
    printIndents(indent);
    printf("begin %s(", funcCall->id->name);
    printFormals(funcCall->formals, indent);
    printf(");\n");
}

void printIndents(int indent){
    for(int i = 0; i < indent; i++){
        printf("\t");
    }
}